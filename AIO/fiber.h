//
// Created by rao on 3/3/24.
//

#ifndef AIO_FIBER_H
#define AIO_FIBER_H


#include <ucontext.h>
#include <memory>
#include <functional>
#include "thread.h"


namespace AIO {


    class Fiber : public std::enable_shared_from_this<Fiber> {
    public:
        typedef std::shared_ptr<Fiber> ptr;
        enum State {
            INIT,
            HOLD,
            EXEC,
            TERM,
            READY,
            EXCEPT,
        };
    private:
        Fiber();

    public:
        Fiber(std::function<void()> cb, size_t stacksize = 0);

        ~Fiber();

        //重置协程函数，并重制状态
        void reset(std::function<void()> cb);

        //切换到当前协程执行
        void swapIn();

        //切换到后台执行
        void swapOut();

        uint64_t getId() const { return m_id; };


    public:
        //设置当前协程
        static void SetThis(Fiber *f);

        // 返回当前协程
        static Fiber::ptr GetThis();

        //协程到后台，设置为Ready状态
        static void YieldToReady();

        static void YieldToHold();

        static uint64_t TotalFibers();

        static void MainFunc();

        static uint64_t GetFiberId();

    private:
        uint64_t m_id = 0;
        uint32_t m_stacksize = 0;
        State m_state = INIT;

        ucontext_t m_ctx;

        void *m_stack = nullptr;

        std::function<void()> m_cb;


    };

} // AIO

#endif //AIO_FIBER_H
