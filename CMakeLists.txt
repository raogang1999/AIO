cmake_minimum_required(VERSION 3.22.0)
project(AIO)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)

set(LIB_SRC
        AIO/log.cc
        AIO/util.cc
        AIO/config.cc
        AIO/thread.cc
)

add_library(AIO SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(AIO) #__FILE__
#add_library(AIO_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES(AIO_static PROPERTIES OUTPUT_NAME "AIO")

set(LIBS
        AIO
        pthread
        yaml-cpp
)

add_executable(test tests/test.cc)
force_redefine_file_macro_for_sources(test) #__FILE__
add_dependencies(test AIO)
target_link_libraries(test ${LIBS})

add_executable(test_config tests/test_config.cc)
force_redefine_file_macro_for_sources(test_config) #__FILE__
add_dependencies(test_config AIO)
target_link_libraries(test_config ${LIBS})


add_executable(test_thread tests/test_thread.cc)
force_redefine_file_macro_for_sources(test_thread) #__FILE__
add_dependencies(test_thread AIO)
target_link_libraries(test_thread ${LIBS})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

