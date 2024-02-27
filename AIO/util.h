#ifndef __AIO__UTIL_H__

#define __AIO__UTIL_H__


#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<cstdint>

namespace AIO {

    pid_t GetThreadId();

    uint32_t GetFiberId();


}


#endif