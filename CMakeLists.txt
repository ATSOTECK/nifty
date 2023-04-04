cmake_minimum_required(VERSION 3.21)
project(nifty)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    if(NOT DEFINED ARCH)
        message(WARNING "Setting ARCH to x86_64 by default.")
        set(ARCH "x86_64") #Intel by default.
    endif()

    set(CMAKE_OSX_ARCHITECTURES ${ARCH})
    message(STATUS "Mac arch ${CMAKE_OSX_ARCHITECTURES}")
endif ()

execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE CMAKE_CXX_FLAGS)
if (APPLE)
    string(STRIP ${CMAKE_CXX_FLAGS} CMAKE_CXX_FLAGS)
endif ()

find_package(LLVM REQUIRED CONFIG)

# Use WARNING so I can actually see the message in clion.
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

separate_arguments(LLVM_DEFINITIONS)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/inc)

set(LLVM_LINK_COMPONENTS
        AllTargetsAsmParsers
        AllTargetsCodeGens
        AllTargetsDescs
        AllTargetsDisassemblers
        AllTargetsInfos
        Analysis
        AsmPrinter
        BitReader
        Core
        DebugInfoPDB
        InstCombine
        IrReader
        LibDriver
        Linker
        LTO
        MC
        MCDisassembler
        native
        nativecodegen
        Object
        Option
        ScalarOpts
        Support
        Target
        TransformUtils
        WindowsManifest
        WindowsDriver
)

llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})

if (APPLE)
    set(lld_libs ${lld_libs} xar)
endif ()

message(STATUS "linking to llvm libs ${lld_libs}")
message(STATUS "Found lld libs ${lld_libs}")

add_executable(nifty main.cpp src/util/str.cpp src/util/str.hpp src/token.cpp src/token.hpp src/common.hpp src/lexer.cpp src/lexer.hpp src/parser.cpp src/parser.hpp src/node.cpp src/node.hpp src/symbolTable.cpp src/symbolTable.hpp src/project.cpp src/project.hpp src/package.cpp src/package.hpp src/codegen.cpp src/codegen.hpp)

target_link_libraries(nifty ${llvm_libs} ${lld_libs})
