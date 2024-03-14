#include <stdio.h>

#include "util/str.h"

#ifdef N_MAC
#   include <mach-o/dyld.h>
#endif

#ifndef N_WIN
#   include <unistd.h>
#endif

#include <stdint.h>

#include "project.h"
#include "util/help.h"

int main(int argc, char **argv) {
    bool buildFileFound = access(NIFTY_BUILD_FILE, F_OK) == 0;
    ProjectInfo projectInfo = { .loaded = false };
    if (buildFileFound) {
        projectInfo = loadProject();
    }

    if (argc >= 2) {
        string cmd = argv[1];

        if (str_eq2(cmd, "help", "-h")) {
            printHelp(argv[2]);
        } else if (str_eq2(cmd, "version", "-v")) {
            printf("%s\n", NIFTY_VERSION);
        } else if (str_eq2(cmd, "info", "-i")) {
            string installedLocation = str_new("unknown", nullptr);
#ifdef N_WIN
            uint32_t size = 1024;
            string loc = str_new_empty(size);
            GetModuleFileNameA(nullptr, loc, 1024);
            installedLocation = str_copy(installedLocation, loc);
            str_delete(loc);
#endif
#ifdef N_MAC
            uint32_t size = 1024;
            string loc = str_new_empty(size);
            if (_NSGetExecutablePath(loc, &size) == 0) {
                installedLocation = str_copy(installedLocation, loc);
                str_delete(loc);
            }
#endif

            printf("Nifty version %s\nBuilt on %s\n", NIFTY_VERSION, NIFTY_DATE);
            printf("Installed location: %s\n", installedLocation);
            str_delete(installedLocation);
        } else if (str_eq2(cmd, "list", "-l")) {
            listTargets(projectInfo);
        } else if (str_eq2(cmd, "build", "-b")) {
            build(argv[2], projectInfo);
        } else if (str_eq2(cmd, "run", "-r")) {
            run(argv[2], projectInfo);
        } else if (str_eq2(cmd, "new", "-n")) {
            newProject();
        } else if (str_eq2(cmd, "test", "-t")) {
            printf("test\n");
        } else {
            printf("unknown\n");
        }
    } else {
        if (!buildFileFound) {
            printf("No '%s' file found.\n", NIFTY_BUILD_FILE);
            printf("Run 'nifty help' for help.\nExiting.\n");
        }
    }

    return 0;
}
