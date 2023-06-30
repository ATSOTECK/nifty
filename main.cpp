/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2022 - 2023 Skyler Burwell
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#include <iostream>

#include "common.hpp"
#include "project.hpp"

#include <filesystem>
#include <fstream>

#ifdef N_MAC
#   include <mach-o/dyld.h>
#endif

void unknownCmd(const String &cmd) {
    if (!cmd.empty()) {
        db("Unknown command '" << cmd << "'.");
        db("See 'nifty help' for a list of valid commands.");
    }
}

void printBuildFlags() {
    hlp("-name <string> " << " Set the name of the compiled executable. Overwrites what is in the build file.");
    hlp("-opt <string> " << " Set the optimization level. Valid options are: none, debug, size, fast");
    hlp2("" << " none:  (default) Does not run any optimizations at all.");
    hlp2("" << " debug: Does not optimize and includes debug symbols.");
    hlp2("" << " size:  Optimizes for size rather than speed.");
    hlp2("" << " fast:  Optimizes for speed.");
    hlp("-debug " << " Enable debug symbols.");
    hlp("-keep-tmp " << " Keeps all temporary files generated during the compile.");
    hlp("-emit-llvm " << " Emits the LLVM IR generated during the compile.");
    hlp("-no-color " << " Disable the use of colors in the error/warning messages.");
    hlp("-yes-color " << " Always use colors in the error/warning messages.");
    hlp("-define <string> " << " Defines a macro that can be used in compilation. Can set optional value.");
    hlp2("" << " -define TRACE_EXECUTION");
    hlp2("" << " -define PRINT_DEPTH=3\tNote no spaces are allowed.");
    hlp("-error-on-warn " << " Treat all warnings as errors.");
    hlp("-no-warn " << " Disable all warnings.");
    hlp("-no-entry " << " Tells the compiler there is no default entry point.");
}

void printHelp(const String &cmd) {
    db("Nifty Programming Language");
    dbln

    if (cmd.empty()) {
        db("Usage: nifty [command] <inputs> <flags>");
        dbln
        db("Commands:");
        hlp("build: " << " Build the default or specified target.");
        hlp("run: " << " Build and run the default or specified target.");
        hlp("clean: " << " Delete compiled files for the project or target.");
        hlp("help: " << " Show this help information or information on the specified command.");
        hlp("new: " << " Create new Nifty project.");
        hlp("test: " << " Compile and run all functions with the test attribute in the default or specified target.");
        hlp("docs: " << " Generate documentation from the default or specified target.");
        hlp("version: " << " Prints Nifty version info.");
        hlp("info: " << " Prints information about the Nifty compiler.");
        hlp("list: " << " List all of the targets for the build file in the current directory.");
        dbln
        db("If no command is given Nifty will build and run the default target from '" << NIFTY_BUILD_FILE << "'.");
    }

    if (cmd == "build") {
        db("Usage: nifty build [arguments]");
        hlp("nifty build <string>: " << " Build the default or specified target.");
        dbln
        db("Examples:");
        hlp("nifty build " << " Builds the default target in build.toml");
        hlp("nifty build filename.nifty " << " Builds 'filename.nifty'");
        hlp("nifty build target " << " Builds 'target' in build.toml");
        dbln
        db("Flags:");
        hlp("-all " << " Builds all of the targets in the build file.");
        printBuildFlags();
        return;
    }
    
    if (cmd == "run") {
        db("Usage: nifty run [arguments]");
        hlp("nifty run <string>: " << " Build and run the default or specified target.");
        dbln
        db("Examples:");
        hlp("nifty run " << " Builds and runs the default target in build.toml");
        hlp("nifty run filename.nifty " << " Builds and runs 'filename.nifty'");
        hlp("nifty run target " << " Builds and runs 'target' in build.toml");
        dbln
        db("Flags:");
        printBuildFlags();
        return;
    }
    
    if (cmd == "clean") {
        db("Usage: nifty clean [targetName]");
        hlp("nifty clean <string>: " << "Delete compiled files for the project or target.");
        dbln
        hlp("nifty clean " << " Cleans the default target in build.toml.");
        hlp("nifty clean target " << " Cleans 'target' in build.toml.");
        dbln
        db("Flags:");
        hlp("-all " << " Cleans all of the targets in the build file.");
        return;
    }

    if (cmd == "help" || cmd == "h") {
        db("Please seek professional help.");
        return;
    }

    if (cmd == "new" || cmd == "n") {
        hlp("nifty new " << " Creates a new nifty project.");
        dbln
        db("Project Info:");
        hlp("Project name " << " Name of the project. May not contain whitespace.");
        hlp("Entry point " << " The file containing the default entry point. May not contain whitespace.");
        hlp("Author " << " Name of the person or organization developing the project. Optional.");
        hlp("License " << " The license for this project. A license.md file will be autogenerated. Optional");
        dbln
        db("Licences:");
        hlp("(SPDX short identifier) " << " (Description)");
        hlp("Apache-2.0 " << " Apache License, Version 2.0");
        hlp("BSD-3-Clause " << " The 3-Clause (New) BSD License also called Modified BSD License");
        hlp("BSD-2-Clause " << " The 2-Clause (Simplified) BSD License also called FreeBSD License");
        hlp("GPL-2.0 " << " GNU General Public License version 2");
        hlp("GPL-3.0-only " << " GNU General Public License version 3");
        hlp("LGPL-2.1 " << " GNU Lesser General Public License version 2.1");
        hlp("LGPL-3.0 " << " GNU Lesser General Public License version 3");
        hlp("MIT " << " The MIT License");
        hlp("MPL-2.0 " << " Mozilla Public License 2.0");
        hlp("CDDL-1.0 " << " Common Development and Distribution License 1.0");
        hlp("EPL-2.0 " << " Eclipse Public License version 2.0");
        hlp("Zlib " << " The zlib/libpng License (Zlib)");
        hlp("License identifiers are not case sensitive.");
        hlp("Other licenses can be used but you must provide them.");
        return;
    }

    if (cmd == "version" || cmd == "v") {
        db("Prints version info.");
        return;
    }
    
    if (cmd == "info" || cmd == "i") {
        db("Prints compiler info.");
        return;
    }

    if (cmd == "me") {
        db("I wish I could :(");
        return;
    }
    
    if (cmd == "test") {
        db("Usage: nifty test [targetName]");
        hlp("nifty test <string>: " << " Compile and run all functions with the test attribute in the default or specified target.");
        dbln
        hlp("nifty test " << " Runs tests in the default target in build.toml.");
        hlp("nifty test target " << " Runs tests in 'target' in build.toml.");
        dbln
        db("Flags:");
        hlp("-all " << " Runs tests for all of the targets in the build file.");
        return;
    }
    
    if (cmd == "docs") {
        db("Usage: nifty docs [targetName]");
        hlp("nifty docs <string>: " << " Generate documentation from the default or specified target.");
        dbln
        hlp("nifty docs " << " Generate docs for the default target in build.toml.");
        hlp("nifty docs target " << " Generate docs for 'target' in build.toml.");
        dbln
        db("Flags:");
        hlp("-all " << " Generate docs for all of the targets in the build file.");
        return;
    }
    
    if (cmd == "definition") {
        db("nifty\tnif*ty");
        hlp("adjective " << " Particularly good, skillful, or effective.");
        hlp("adjective " << " Fashionable; stylish.");
        return;
    }

    unknownCmd(cmd);
}

// TODO: Get build args.
void build(const char *arg) {
    Project::build(arg);
}

void run(const char *arg) {
    Project::run(arg);
}

void newProject(bool existingProject) {
    std::string answer;
    
    if (existingProject) {
        db("A Nifty project already exists in this directory.");
        dbnln("Would you like to overwrite it? (no) >");
        std::getline(std::cin, answer);
        std::transform(answer.begin(), answer.end(), answer.begin(), [](unsigned char c){return std::tolower(c);});
    
        if (answer.empty() && answer != "yes" && answer != "y") {
            db("Exiting.");
            return;
        }
        
        answer.clear();
        dbln
    }
    
    CreateProjectInfo info{};
    
    db("This utility will help you make a new Nifty project.");
    db("See 'nifty help new' for more information.");
    dbln
    
    dbnln("Project name: (Untitled) >");
    std::getline(std::cin, info.name);
    
    dbnln("Project version: (0.1.0) >");
    std::getline(std::cin, info.version);
    
    dbnln("Entry point: (main.nifty) >");
    std::getline(std::cin, info.entryPoint);
    
    dbnln("Author: >");
    std::getline(std::cin, info.author);
    
    dbnln("License: (zlib) >");
    std::getline(std::cin, info.license);
    
    if (info.name.empty()) {
        info.name = "Untitled";
    }
    
    if (info.version.empty()) {
        info.version = "0.1.0";
    }
    
    if (info.entryPoint.empty()) {
        info.entryPoint = "main.nifty";
    }
    
    if (info.license.empty()) {
        info.license = "zlib";
    }
    
    Project::print(info);
    
    dbln
    dbnln("Is this ok? (yes) >");
    std::getline(std::cin, answer);
    
    std::transform(answer.begin(), answer.end(), answer.begin(), [](unsigned char c){return std::tolower(c);});
    if (!answer.empty() && answer != "yes" && answer != "y") {
        dbln
        newProject(false);
        return;
    }
    
    Project::create(info);
}

int main(int argc, char **argv) {
    std::ifstream buildFile;
    buildFile.open(NIFTY_BUILD_FILE, std::ios_base::in);
    bool buildFileFound = buildFile.is_open();
    if (buildFileFound) {
        buildFile.close();
    }

    if (argc >= 2) {
        String cmd = argv[1];

        if (cmd == "help" || cmd == "-h") {
            argc == 3 ? printHelp(argv[2]) : printHelp("");
        } else if (cmd == "version" || cmd == "-v") {
            db(NIFTY_VERSION);
        } else if (cmd == "info" || cmd == "-i") {
            String installedLocation = "???";
#ifdef N_WIN
            char *loc = (char*)malloc(sizeof(char) * 1024);
            GetModuleFileNameA(nullptr, loc, 1024);
            installedLocation = loc;
            free(loc);
#endif
#ifdef N_MAC
            uint32 size = 1024;
            char *loc = (char*)malloc(sizeof(char) * size);
            if (_NSGetExecutablePath(loc, &size) == 0) {
                installedLocation = loc;
                free(loc);
            }
#endif
#ifdef N_LINUX
            installedLocation = std::filesystem::canonical("/proc/self/exe").string();
#endif
            
            // std::filesystem::current_path()
            db("Nifty version " << NIFTY_VERSION << "\nBuilt on " << NIFTY_DATE);
            db("Installed location: " << installedLocation);
        } else if (cmd == "list" || cmd == "-l") {
            if (buildFileFound) {
                Project::listTargets();
                return 0;
            }
            
            db("No build file found.\nExiting.");
        } else if (cmd == "build" || cmd == "-b") {
            build(argv[2]);
        } else if (cmd == "run" || cmd == "-r") {
            run(argv[2]);
        } else if (cmd == "new" || cmd == "-n") {
            newProject(buildFileFound);
        } else {
            unknownCmd(cmd);
        }
    } else {
        if (!buildFileFound) {
            db("No '" << NIFTY_BUILD_FILE << "' file found.");
            db("Run 'nifty help' for help.\nExiting.");
        } else {
            run(nullptr);
        }
    }

    return 0;
}

/*
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>

#include "buildConfig.hpp"

int main() {
    let context = new llvm::LLVMContext();
    llvm::IRBuilder<> builder(*context);
    let mod = std::make_unique<llvm::Module>("test", *context);

    let s32 = builder.getInt32Ty();
    let prototype = llvm::FunctionType::get(s32, false);
    let mainFn = llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, "main", mod.get());
    let body = llvm::BasicBlock::Create(*context, "body", mainFn);
    builder.SetInsertPoint(body);

    let s8p = builder.getInt8PtrTy();
    let printfPrototype = llvm::FunctionType::get(s8p, true);
    let printfFn = llvm::Function::Create(printfPrototype, llvm::Function::ExternalLinkage, "printf", mod.get());

    let str = builder.CreateGlobalStringPtr(llvm::StringRef("hello world!\n"), "hello", 0, mod.get());
    builder.CreateCall(printfFn, { str });

    let ret = llvm::ConstantInt::get(s32, 0);
    builder.CreateRet(ret);
//
    let engine = llvm::EngineBuilder(std::move(mod)).setEngineKind(llvm::EngineKind::Interpreter).create();
    let mainFunc = engine->FindFunctionNamed(llvm::StringRef("main"));
    let res = engine->runFunction(mainFunc, {});

    return (int)res.IntVal.getLimitedValue();
//

    mod->print(llvm::errs(), nullptr);

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    let config = getDefaultBuildConfig();

    let targetTriple = llvm::sys::getDefaultTargetTriple();
    mod->setTargetTriple(targetTriple);

    std::string err;
    let target = llvm::TargetRegistry::lookupTarget(targetTriple, err);

    if (!target) {
        llvm::errs() << err;
        return 1;
    }

    std::string CPU = "generic";
    std::string features;

    llvm::TargetOptions opt;
    let rm = llvm::Optional<llvm::Reloc::Model>();
    let targetMachine = target->createTargetMachine(targetTriple, CPU, features, opt, rm);

    mod->setDataLayout(targetMachine->createDataLayout());

    let filename = config.objectName.stdString();
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

    if (ec) {
        llvm::errs() << "Could not open file:" << ec.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    let filetype = llvm::CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, filetype)) {
        llvm::errs() << "targetMachine can't emit a file of this type.";
        return 1;
    }

    pass.run(*mod);
    dest.flush();

    // call cc NiftyDefaultOutput -o NiftyOutExe

    llvm::outs() << "Wrote " << filename << "\n";

    return 0;
}

*/
