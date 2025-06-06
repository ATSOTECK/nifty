cmake_minimum_required(VERSION 3.30)
project(nifty)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/inc)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(nifty src/main.cpp
        src/common.hpp
        src/project.cpp
        src/project.hpp
        src/util.cpp
        src/util.hpp
        src/help.cpp
        src/help.hpp
        src/compiler.cpp
        src/compiler.hpp)

target_link_libraries(nifty fmt)

target_link_libraries(nifty libgit2.a)

target_link_libraries(nifty "-framework CoreFoundation")
target_link_libraries(nifty "-framework Security")
set_target_properties(nifty PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")

target_link_libraries(nifty libiconv.dylib)
target_link_libraries(nifty libz.dylib)
