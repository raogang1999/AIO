//
// Created by rao on 3/2/24.
//
#include "AIO/AIO.h"
#include <unistd.h>

AIO::Logger::ptr g_logger = AIO_LOG_ROOT();

int count = 0;
//AIO::RWMutex s_mutex;
AIO::Mutex mutex;

void fun1() {
    AIO_LOG_INFO(g_logger) << " name: " << AIO::Thread::GetName()
                           << " this.name" << AIO::Thread::GetThis()->getName()
                           << " id: " << AIO::GetThreadId()
                           << " this.id: " << AIO::Thread::GetThis()->getId();
    for (int i = 0; i < 1000000; ++i) {
//        AIO::RWMutex::ReadLock lock(s_mutex);
        AIO::Mutex::Lock lock(mutex);
        ++count;
    }

}

void fun2() {
    while (true) {
        AIO_LOG_INFO(g_logger) << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    }
}

void fun3() {
    while (true) {
        AIO_LOG_INFO(g_logger) << "==============================================================";
    }
}

int main(int argc, char **argv) {
    AIO::CASLock cas;

    AIO::CASLock::Lock lock(cas);


    AIO_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/rao/workspace/AIO/bin/conf/log2.yml");
    AIO::Config::LoadFromYaml(root);
    std::vector<AIO::Thread::ptr> thrs;
    for (int i = 0; i < 2; i++) {
        AIO::Thread::ptr thr(new AIO::Thread(&fun2, "name_" + std::to_string(i * 2)));
        AIO::Thread::ptr thr2(new AIO::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr);
        thrs.push_back(thr2);
    }
    for (size_t i = 0; i < thrs.size(); i++) {
        thrs[i]->join();
    }
    AIO_LOG_INFO(g_logger) << "thread test end";

    AIO_LOG_INFO(g_logger) << "count = " << count;



    return 0;
}