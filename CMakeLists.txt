cmake_minimum_required(VERSION 3.3)
project(gbhd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cc util.h)
add_executable(gbhd ${SOURCE_FILES})