cmake_minimum_required(VERSION 3.1)

project(protodef)
set(CMAKE_CXX_STANDARD 17)

add_executable(step0_repl step0_repl.cpp)
add_executable(step1_read_print step1_read_print.cpp reader.cpp printer.cpp)
add_executable(step2_eval step2_eval.cpp reader.cpp printer.cpp)
add_executable(step3_env step3_env.cpp reader.cpp printer.cpp)
add_executable(step4_if_fn_do step4_if_fn_do.cpp reader.cpp printer.cpp core.cpp)
