/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2024 Skyler Burwell
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
        if (buildFileFound) {
            run(nullptr, projectInfo);
        } else {
            println("Nifty v%s :: %s", NIFTY_VERSION, NIFTY_DATE);
            println("No '%s' file found.", NIFTY_BUILD_FILE);
            println("Run 'nifty help' for a list of commands. Exiting.");
        }
    }

    freeProject(projectInfo);

    return 0;
}
