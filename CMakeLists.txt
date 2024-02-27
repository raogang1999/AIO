cmake_minimum_required(VERSION 3.22)
project(AIO)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")



set(LIB_SRC 
    AIO/log.cc
    AIO/util.cc
    )

add_library(AIO SHARED ${LIB_SRC})
#add_library(AIO_static STATIC ${LIB_SRC})

#SET_TARGET_PROPERTIES(AIO_static PROPERTIES OUTPUT_NAME "AIO")

add_executable(test tests/test.cc)
add_dependencies(test AIO)
target_link_libraries(test AIO)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

