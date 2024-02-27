//
// Created by Rao on 2024/2/26.
//
#include <iostream>
#include<thread>
#include "../AIO/log.h"
#include "../AIO/util.h"

int main(int argc, char **argv) {
    AIO::Logger::ptr logger(new AIO::Logger("rao"));
    logger->addAppender(AIO::LogAppender::ptr(new AIO::StdoutLogAppender));
    AIO::FileLogAppender::ptr file_appender(new AIO::FileLogAppender("./log.txt"));
    AIO::LogFormatter::ptr fmt(new AIO::LogFormatter("%d%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(AIO::LogLevel::ERROR);
    logger->addAppender(file_appender);


    // AIO::LogEvent::ptr event(new AIO::LogEvent(__FILE__,__LINE__,0,AIO::GetThreadId(),AIO::GetFiberId(),time(0)));
    // logger->log(AIO::LogLevel::FATAL,event);
    std::cout << "hello AIO Log" << std::endl;
    GANG_LOG_DEBUG(logger) << " any thing ?";
    GANG_LOG_INFO(logger) << " infomation ";
    GANG_LOG_FATAL(logger) << " Fatal ";

    GANG_LOG_FMT_INFO(logger, "test fmt %s", "abc");

    auto l = AIO::LoggerMgr::GetInstance()->getLogger("xx");
    GANG_LOG_DEBUG(l) << "xxx";
    return 0;
}