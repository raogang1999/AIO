//
// Created by rao on 3/3/24.
//


#include <iostream>
#include "AIO/AIO.h"
#include <assert.h>

AIO::Logger::ptr g_logger = AIO_LOG_ROOT();


void test_assert() {
//    AIO_LOG_INFO(g_logger) << AIO::BacktraceToString(10);
    AIO_ASSERT2(1 == 0, "ABCDEF XX");

}

int main() {
    test_assert();

    return 0;

}