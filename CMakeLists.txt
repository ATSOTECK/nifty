cmake_minimum_required(VERSION 3.22)
project(nifty C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_executable(nifty src/main.c src/common.h src/util/str.h src/util/str.c src/project.h inc/toml/toml.c inc/toml/toml.h src/project.c src/util/help.h src/util/help.c src/lexer.h src/lexer.c src/testing.h src/parser.h src/parser.c)
