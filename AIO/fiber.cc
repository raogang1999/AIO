//
// Created by rao on 3/3/24.
//

#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "log.h"
#include <atomic>


namespace AIO {
    static Logger::ptr g_logger = AIO_LOG_NAME("system");
    static std::atomic<uint64_t> s_fiber_id{0};
    static std::atomic<uint64_t> s_fiber_count{0};

    //当前执行协程
    static thread_local Fiber *t_fiber = nullptr;
    //线程主协程
    static thread_local Fiber::ptr t_threadFiber = nullptr;

    static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
            Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");


    class MallocStackAllocator {
    public:
        static void *Alloc(size_t size) {
            return malloc(size);
        }

        static void Dealloc(void *vp, size_t size) {
            return free(vp);
        }
    };

    using StackAllocator = MallocStackAllocator;

    uint64_t Fiber::GetFiberId() {
        if (t_fiber) {
            return t_fiber->getId();
        }
        return 0;
    }

    //主协程
    Fiber::Fiber() {
        m_state = EXEC;
        SetThis(this);

        if (getcontext(&m_ctx)) {
            AIO_ASSERT2(false, "getcontext");
        }
        ++s_fiber_count;

        AIO_LOG_DEBUG(g_logger) << " Fiber::Fiber";

    }

    Fiber::Fiber(std::function<void()> cb, size_t stacksize)
            : m_id(++s_fiber_id), m_cb(cb) {
        ++s_fiber_count;
        m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
        m_stack = StackAllocator::Alloc(m_stacksize);
        if (getcontext(&m_ctx)) {
            AIO_ASSERT2(false, "getcontext");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
        AIO_LOG_DEBUG(g_logger) << " Fiber::Fiber id = " << m_id;

    }

    Fiber::~Fiber() {
        --s_fiber_count;
        if (m_stack) {
            AIO_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
            StackAllocator::Dealloc(m_stack, m_stacksize);
        } else {
            //是主协程
            AIO_ASSERT(!m_cb);
            AIO_ASSERT(m_state == EXEC);

            Fiber *cur = t_fiber;
            if (cur == this) {
                SetThis(nullptr);
            }
        }
        AIO_LOG_DEBUG(g_logger) << " Fiber::~Fiber id = " << m_id;


    }

    void Fiber::reset(std::function<void()> cb) {
        AIO_ASSERT(m_stack);
        AIO_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
        m_cb = cb;
        if (getcontext(&m_ctx)) {
            AIO_ASSERT2(false, "getcontex");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
        m_state = INIT;
    }

    //从主协程到 子协程
    void Fiber::swapIn() {
        //操作前 做一些工作
        SetThis(this);
        AIO_ASSERT(m_state != EXEC);

        //交换
        //上下文切换
        if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
            AIO_ASSERT2(false, "swapcontext");
        }
    }

    //  切换到后台，主协程执行
    void Fiber::swapOut() {
        SetThis(t_threadFiber.get());
        if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
            AIO_ASSERT2(false, "swapcontext");
        }
    }

    void Fiber::SetThis(Fiber *f) {
        t_fiber = f;
    }

    Fiber::ptr Fiber::GetThis() {
        if (t_fiber) {
            return t_fiber->shared_from_this();
        }
        //不存在协程，则创建主协程
        Fiber::ptr main_fiber(new Fiber);
       
        AIO_ASSERT(t_fiber == main_fiber.get());

        t_threadFiber = main_fiber;

        return t_fiber->shared_from_this();
    }

    void Fiber::YieldToReady() {
        Fiber::ptr cur = GetThis();
        cur->m_state = READY;
        cur->swapOut();
    }

    void Fiber::YieldToHold() {
        Fiber::ptr cur = GetThis();
        cur->m_state = HOLD;
        cur->swapOut();
    }

    uint64_t Fiber::TotalFibers() {
        return s_fiber_count;
    }

    void Fiber::MainFunc() {
        //创建主协程
        Fiber::ptr cur = GetThis();
        //创建失败则处理
        AIO_ASSERT(cur);

        try {
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        } catch (std::exception &ex) {
            cur->m_state = EXCEPT;
            AIO_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();

        } catch (...) {
            cur->m_state = EXCEPT;
            AIO_LOG_ERROR(g_logger) << "Fiber Except";
        }

        auto raw_ptr = cur.get();
        cur.reset();//引用计数减一
        raw_ptr->swapOut();
        cur->swapOut();//回到调用的地方。

        AIO_ASSERT2(false, "never arrive here");


    }


} // AIO