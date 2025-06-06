
#include "common.hpp"

#include "help.hpp"
#include "project.hpp"
#include "util.hpp"

#include <filesystem>

#ifdef N_MAC
#   include <mach-o/dyld.h>
#endif

#ifndef N_WIN
#   include <unistd.h>
#endif

static void print_colors(const ProjectInfo &info) {
    if (info.config.disableColors) {
        println("Use of colors is disabled.");
        return;
    }

    for (int i = 0; i < 256; ++i) {
        println("\033[38;5;{}m The quick brown fox jumped over the lazy dog. {}", i, i);
    }
    printf(RESET_COLOR);
}

int main(const int argc, char **argv) {
    const bool buildFileFound = std::filesystem::exists(NIFTY_BUILD_FILE);
    ProjectInfo projectInfo{};
    if (buildFileFound) {
        projectInfo = loadProject();
    }

    if (argc >= 2) {
        const std::string cmd = argv[1];

        if (cmd == "help" || cmd == "-h") {
            const std::string helpCmd = argv[2] != nullptr ? argv[2] : "";
            print_help(helpCmd);
        } else if (cmd == "version" || cmd == "-v") {
            println("{}", NIFTY_VERSION);
        } else if (cmd == "info" || cmd == "-i") {
            std::string installedLocation = "unknown";
#ifdef N_WIN
            uint32_t size = 1024;
            string loc = str_new_empty(size);
            GetModuleFileNameA(nullptr, loc, 1024);
            installedLocation = loc;
            str_delete(loc);
#endif
#ifdef N_MAC
            uint32_t size = 1024;
            char *loc = str_new_empty(size);
            if (_NSGetExecutablePath(loc, &size) == 0) {
                installedLocation = loc;
                str_delete(loc);
            }
#endif

            println("Nifty version v{}\nBuilt on {}", NIFTY_VERSION, NIFTY_DATE);
            println("Installed location: {}", installedLocation);
        } else if (cmd == "list" || cmd == "-l") {
            list_targets(projectInfo);
        } else if (cmd == "build" || cmd == "-b") {
            const std::string target = argv[2] != nullptr ? argv[2] : "";
            build(target, projectInfo);
        } else if (cmd == "run" || cmd == "-r") {
            const std::string target = argv[2] != nullptr ? argv[2] : "";
            run(target, projectInfo);
        } else if (cmd == "new" || cmd == "-n") {
            bool noGit = false;
            for (int i = 2; i < argc; ++i) {
                if (str_eq(argv[i], "--no-git")) {
                    noGit = true;
                }
            }

            new_project(buildFileFound, noGit);
        } else if (cmd == "test" || cmd == "-t") {
            println("test");
        } else if (cmd == "colors") {
            print_colors(projectInfo);
        } else {
            println("Unknown command '{}'.\n", cmd);
            print_help("");
        }
    } else {
        if (buildFileFound) {
            run("", projectInfo);
        } else {
            println("Nifty v{} :: {}", NIFTY_VERSION, NIFTY_DATE);
            println("No '{}' file found.", NIFTY_BUILD_FILE);
            println("Run 'nifty help' for a list of commands. Exiting.");
        }
    }

    return 0;
}