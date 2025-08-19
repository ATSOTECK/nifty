cmake_minimum_required(VERSION 3.22)
project(nifty C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/inc)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(nifty src/main.c src/common.h src/util/str.h src/util/str.c src/project.h inc/toml/toml.c inc/toml/toml.h src/project.c src/util/help.h src/util/help.c src/lexer.h src/lexer.c src/testing.h src/parser.h src/parser.c
        src/symbol.h
        src/symbol.c
        src/table.h
        src/table.c)

target_link_libraries(nifty libgit2.a)

target_link_libraries(nifty "-framework CoreFoundation")
target_link_libraries(nifty "-framework Security")
set_target_properties(nifty PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")

target_link_libraries(nifty libiconv.dylib)
target_link_libraries(nifty libz.dylib)

