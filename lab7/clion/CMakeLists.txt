cmake_minimum_required(VERSION 3.3)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c functions.c functions.c.h)
add_executable(clion ${SOURCE_FILES})