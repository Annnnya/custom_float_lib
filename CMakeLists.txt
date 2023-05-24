cmake_minimum_required(VERSION 3.24)
project(custom_float)

set(CMAKE_CXX_STANDARD 17)

add_library(custom_float INTERFACE)

target_include_directories(custom_float INTERFACE my_float.h)

add_executable(lib_test lib_test.cpp)

target_link_libraries(lib_test PRIVATE custom_float)
