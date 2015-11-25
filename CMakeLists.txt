cmake_minimum_required(VERSION 3.3)
project(colorboy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cc mmu.cc cpu.cc util.h register_set.h mmu.h instruction_set.h cpu.h)
add_executable(colorboy ${SOURCE_FILES})