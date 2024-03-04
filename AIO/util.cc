
#include"util.h"
#include "log.h"
#include "fiber.h"

#include<execinfo.h>

namespace AIO {
    AIO::Logger::ptr g_logger = AIO_LOG_NAME("system");

    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberId() {
        return AIO::Fiber::GetFiberId();;
    }

    void Backtrace(std::vector<std::string> &bt, int size, int skip) {
        void **array = (void **) malloc((sizeof(void *) * size));
        size_t s = ::backtrace(array, size);
        char **string = backtrace_symbols(array, s);
        if (string == NULL) {
            AIO_LOG_ERROR(g_logger) << "backtrace_symbols error";
            return;
        }
        for (size_t i = skip; i < s; i++) {
            bt.push_back(string[i]);
        }
        free(string);
        free(array);

    }

    std::string BacktraceToString(int size, int skip, const std::string &prefix) {
        std::vector<std::string> bt;
        Backtrace(bt, size, skip);
        std::stringstream ss;
        for (size_t i = 0; i < bt.size(); i++) {
            ss << prefix << bt[i] << std::endl;
        }
        return ss.str();
    }


} // namespace AIO

