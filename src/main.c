#include <stdio.h>

#include "util/str.h"

#ifdef N_MAC
#   include <mach-o/dyld.h>
#endif

#include <stdlib.h>

void printHelp() {
    printf("you want help\n");
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        char *cmd = argv[1];

        if (str_eq(cmd, "help") || str_eq(cmd, "-h")) {
            printHelp();
        } else if (str_eq(cmd, "version") || str_eq(cmd, "-v")) {
            printf("%s\n", NIFTY_VERSION);
        } else if (str_eq(cmd, "info") || str_eq(cmd, "-i")) {
            char *installedLocation = str_new("unknown", nullptr);

            uint32_t size = 1024;
            char *loc = str_new_empty(size);
            if (_NSGetExecutablePath(loc, &size) == 0) {
                str_copy(installedLocation, loc);
                str_delete(loc);
            }

            printf("Nifty version %s\nBuilt on %s\n", NIFTY_VERSION, NIFTY_DATE);
            printf("Installed location: %s\n", installedLocation);
        }
    }

    return 0;
}
