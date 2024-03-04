//
// Created by rao on 3/3/24.
//

#ifndef AIO_MACRO_H
#define AIO_MACRO_H

#include<string.h>
#include "util.h"
#include <assert.h>

#define  AIO_ASSERT(x) \
if(!(x)){              \
    AIO_LOG_ERROR(AIO_LOG_ROOT())<<"ASSERTION: " #x \
    << "\n backtrace:\n"                            \
    << AIO::BacktraceToString(100,2,"    ");        \
    assert(x);\
}

#define  AIO_ASSERT2(x, w) \
if(!(x)){              \
    AIO_LOG_ERROR(AIO_LOG_ROOT())<<"ASSERTION: " #x \
    << "\n" << w                     \
    << "\n backtrace:\n"                            \
    << AIO::BacktraceToString(100,2,"    ");        \
    assert(x);\
}


#endif //AIO_MACRO_H
