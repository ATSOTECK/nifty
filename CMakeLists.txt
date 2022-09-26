cmake_minimum_required(VERSION 3.21)
project(nifty)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(nifty main.cpp src/util/str.cpp src/util/str.hpp src/token.cpp src/token.hpp src/common.hpp src/lexer.cpp src/lexer.hpp src/parser.cpp src/parser.hpp src/node.cpp src/node.hpp src/symbolTable.cpp src/symbolTable.hpp src/project.cpp src/project.hpp)
