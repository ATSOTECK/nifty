cmake_minimum_required(VERSION 3.24)
project(nifty C)

set(CMAKE_C_STANDARD 99)

add_executable(nifty src/main.c src/common.h src/util/str.h src/util/str.c)
