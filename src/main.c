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
    ProjectInfo *projectInfo = nullptr;
    if (buildFileFound) {
        projectInfo = loadProject();
    }

    if (argc >= 2) {
        string cmd = argv[1];

        if (str_eq2(cmd, "help", "-h")) {
            printHelp(argv[2]);
        } else if (str_eq2(cmd, "version", "-v")) {
            println("%s", NIFTY_VERSION);
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

            println("Nifty version %s\nBuilt on %s", NIFTY_VERSION, NIFTY_DATE);
            println("Installed location: %s", installedLocation);
            str_delete(installedLocation);
        } else if (str_eq2(cmd, "list", "-l")) {
            listTargets(projectInfo);
        } else if (str_eq2(cmd, "build", "-b")) {
            build(argv[2], projectInfo);
        } else if (str_eq2(cmd, "run", "-r")) {
            run(argv[2], projectInfo);
        } else if (str_eq2(cmd, "new", "-n")) {
            newProject(buildFileFound);
        } else if (str_eq2(cmd, "test", "-t")) {
            println("test");
        } else {
            println("unknown");
        }
    } else {
        if (!buildFileFound) {
            println("Nifty v%s :: %s", NIFTY_VERSION, NIFTY_DATE);
            println("No '%s' file found.", NIFTY_BUILD_FILE);
            println("Run 'nifty help' for a list of commands. Exiting.");
        }
    }

    freeProject(projectInfo);

    return 0;
}
