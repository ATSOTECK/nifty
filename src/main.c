#include <stdio.h>

#include "util/str.h"

#ifdef N_MAC
#   include <mach-o/dyld.h>
#endif

#ifndef N_WIN
#   include <unistd.h>
#endif

#include <stdlib.h>
#include <stdint.h>

void printHelp() {
    printf("you want help\n");
}

int main(int argc, char **argv) {
    bool buildFileFound = access(NIFTY_BUILD_FILE, F_OK) == 0;
    if (argc >= 2) {
        string cmd = argv[1];

        if (str_eq(cmd, "help") || str_eq(cmd, "-h")) {
            printHelp();
        } else if (str_eq(cmd, "version") || str_eq(cmd, "-v")) {
            printf("%s\n", NIFTY_VERSION);
        } else if (str_eq(cmd, "info") || str_eq(cmd, "-i")) {
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
                str_copy(installedLocation, loc);
                str_delete(loc);
            }
#endif

            printf("Nifty version %s\nBuilt on %s\n", NIFTY_VERSION, NIFTY_DATE);
            printf("Installed location: %s\n", installedLocation);
            str_delete(installedLocation);
        }
    } else {
        if (!buildFileFound) {
            printf("No '%s' file found.\n", NIFTY_BUILD_FILE);
            printf("Run 'nifty help' for help.\nExiting.\n");
        }
    }

    return 0;
}
