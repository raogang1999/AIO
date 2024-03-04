//
// Created by Rao on 2024/2/26.
//

#ifndef AIO_LOG_H
#define AIO_LOG_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include "singleton.h"
#include <map>
#include "util.h"
#include "thread.h"


#define AIO_LOG_LEVEL(logger, level)\
    if (logger->getLevel() <= level)\
        AIO::LogEventWrap(AIO::LogEvent::ptr(\
        new AIO::LogEvent(logger,level,__FILE__,__LINE__,0,AIO::GetThreadId(),AIO::GetFiberId(),time(0),AIO::Thread::GetName()))).getSS()
#define AIO_LOG_DEBUG(logger) AIO_LOG_LEVEL(logger,AIO::LogLevel::DEBUG)
#define AIO_LOG_INFO(logger) AIO_LOG_LEVEL(logger,AIO::LogLevel::INFO)
#define AIO_LOG_WARN(logger) AIO_LOG_LEVEL(logger,AIO::LogLevel::WARN)
#define AIO_LOG_ERROR(logger) AIO_LOG_LEVEL(logger,AIO::LogLevel::ERROR)
#define AIO_LOG_FATAL(logger) AIO_LOG_LEVEL(logger,AIO::LogLevel::FATAL)

#define AIO_LOG_FMT_LEVEL(logger, level, fmt, ...)\
    if(logger->getLevel()<=level)\
        AIO::LogEventWrap(AIO::LogEvent::ptr(\
        new AIO::LogEvent(logger,level,__FILE__,__LINE__,0,AIO::GetThreadId(),\
        AIO::GetFiberId(),time(0),AIO::Thread::GetName()))).getEvent()->format(fmt,__VA_ARGS__)


#define AIO_LOG_FMT_DEBUG(logger, fmt, ...) AIO_LOG_FMT_LEVEL(logger,AIO::LogLevel::DEBUG,fmt,__VA_ARGS__)
#define AIO_LOG_FMT_INFO(logger, fmt, ...) AIO_LOG_FMT_LEVEL(logger,AIO::LogLevel::INFO,fmt,__VA_ARGS__)
#define AIO_LOG_FMT_WARN(logger, fmt, ...) AIO_LOG_FMT_LEVEL(logger,AIO::LogLevel::WARN,fmt,__VA_ARGS__)
#define AIO_LOG_FMT_ERROR(logger, fmt, ...) AIO_LOG_FMT_LEVEL(logger,AIO::LogLevel::ERROR,fmt,__VA_ARGS__)
#define AIO_LOG_FMT_FATAL(logger, fmt, ...) AIO_LOG_FMT_LEVEL(logger,AIO::LogLevel::FATAL,fmt,__VA_ARGS__)

#define AIO_LOG_ROOT() AIO::LoggerMgr::GetInstance()->getRoot()


#define AIO_LOG_NAME(name) AIO::LoggerMgr::GetInstance()->getLogger(name)

namespace AIO {
    class Logger;

    class LoggerManager;


    //日志级别
    class LogLevel {
    public:
        enum Level {
            UNKNOWN = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };

        static const char *ToString(LogLevel::Level level);

        static LogLevel::Level FromString(const std::string &level);
    };

    //元数据
    class LogEvent {
    public:
        // 方便内存管理
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                 const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id,
                 uint64_t time, const std::string &thread_name);

        const char *getFile() const { return m_file; }

        int32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint32_t getThreadId() const { return m_threadId; }

        const std::string& getThreadName() const { return m_threadName; }

        uint32_t getFiberId() const { return m_fiberId; }

        uint64_t getTime() const { return m_time; }

        std::string getContent() const { return m_ss.str(); }

        std::stringstream &getSS() { return m_ss; }

        std::shared_ptr<Logger> getLogger() { return m_logger; }

        LogLevel::Level getLevel() { return m_level; }

        void format(const char *fmt, ...);

        void format(const char *fmt, va_list al);

    private:
        const char *m_file = nullptr; // 文件名
        int32_t m_line = 0;// 行号
        uint32_t m_elapse;// 重新启动到现在的毫秒数
        uint32_t m_threadId = 0;//线程号
        uint32_t m_fiberId = 0;//协程号
        uint64_t m_time;//时间戳
        std::string m_threadName;//线程名称
        std::stringstream m_ss;//内容

        std::shared_ptr<Logger> m_logger;
        LogLevel::Level m_level;
    };

    //LogEvent 包装器，放LogEvent的智能指针
    class LogEventWrap {
    public:
        LogEventWrap(LogEvent::ptr e);

        ~LogEventWrap();

        std::stringstream &getSS();

        LogEvent::ptr getEvent() { return m_event; }

    private:
        LogEvent::ptr m_event;
    };


    //格式化
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(const std::string &pattern);

        void init();

        //%t %m %n %thread_id
        // 调用具体的 item 去解析
        // 参考log4j
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    public:
        // 解析器
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;

            ~FormatItem() {}

            virtual void
            format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        bool isError() const { return m_error; };

        const std::string getPattern() const { return m_pattern; };
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
        bool m_error = false;
    };

    //输出
    // appender 负责将event 格式化后输出到指定位置
    class LogAppender {
        friend Logger;
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef SpinLock MutexType;


        virtual ~LogAppender() {}

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr val);

        LogFormatter::ptr getFormatter();

        void setLevel(LogLevel::Level level) { m_level = level; };

        LogLevel::Level getLevel() { return m_level; }

        virtual std::string toYamlString() = 0;


    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
        MutexType m_mutex;
        bool m_hasFormatter = false;


    };

    //日志器
    // 日志器相当于总控，提供调用接口，
    // 它底层，调用 具体的appender 使用formatter 将 event 输出到 目的地
    class Logger : public std::enable_shared_from_this<Logger> {
        friend class LoggerManager;

    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef SpinLock MutexType;


        Logger(const std::string &name = "root");

        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);

        void info(LogEvent::ptr event);

        void warn(LogEvent::ptr event);

        void error(LogEvent::ptr event);

        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);

        void delAppender(LogAppender::ptr appender);

        void clearAppender();

        LogLevel::Level getLevel() const { return m_level; }

        void setLevel(LogLevel::Level val) { m_level = val; }

        const std::string &getName() const { return m_name; }

        void setFormatter(LogFormatter::ptr val);

        void setFormatter(const std::string &val);

        LogFormatter::ptr getFormatter();

        std::string toYamlString();


    private:
        std::string m_name;    // 日志名称
        LogLevel::Level m_level;// 日志级别
        std::list<LogAppender::ptr> m_appenders;// 日志输出集合
        LogFormatter::ptr m_formatter;
        //主日志器
        Logger::ptr m_root;
        MutexType m_mutex;

    };

    //具体的appender,控制台
    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

        virtual std::string toYamlString() override;


    };

    //输出到文件
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;

        FileLogAppender(const std::string &filename);

        bool reopen();

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

        virtual std::string toYamlString() override;

    private:
        std::string m_filename;
        std::ofstream m_filestream;
        uint64_t m_lastTime;
    };

    //日志管理器，
    class LoggerManager {
    public:
        typedef SpinLock MutexType;

        LoggerManager();

        Logger::ptr getLogger(const std::string &name);

        void init();

        Logger::ptr getRoot() const { return m_root; }

        std::string toYamlString();

    private:
        std::map<std::string, Logger::ptr> m_loggers;//日志管理器
        Logger::ptr m_root;//默认
        MutexType m_mutex;
    };

    typedef AIO::Singleton<LoggerManager> LoggerMgr;

}

#endif //AIO_LOG_H
