//
// Created by rao on 3/3/24.
//


#include "AIO/AIO.h"

AIO::Logger::ptr g_logger = AIO_LOG_ROOT();

void run_in_fiber() {
    AIO_LOG_INFO(g_logger) << "run_in_fiber begin";
    AIO::Fiber::YieldToHold();
    AIO_LOG_INFO(g_logger) << "run_in_fiber end";
    AIO::Fiber::YieldToHold();

}

void test_fiber() {
    AIO_LOG_INFO(g_logger) << "main begin ";
    {
        AIO::Fiber::GetThis();
        //创建子协程
        AIO::Fiber::ptr fiber(new AIO::Fiber(run_in_fiber));
        fiber->swapIn();
        AIO_LOG_INFO(g_logger) << "main after swapIn ";
        fiber->swapIn();

        AIO_LOG_INFO(g_logger) << "main end ";
        fiber->swapIn();

    }
    AIO_LOG_INFO(g_logger) << "main end2 ";
}

int main() {
    AIO::Thread::SetName("Fiber");

    std::vector<AIO::Thread::ptr> thrs;
    for (int i = 0; i < 3; ++i) {
        thrs.push_back(AIO::Thread::ptr(new AIO::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for (auto i: thrs) {
        i->join();
    }


    return 0;


}