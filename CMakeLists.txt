cmake_minimum_required(VERSION 3.1)

project(protodef)
set(CMAKE_CXX_STANDARD 17)

add_executable(step0_repl step0_repl.cpp)
add_executable(step1_read_print step1_read_print.cpp reader.cpp printer.cpp)
