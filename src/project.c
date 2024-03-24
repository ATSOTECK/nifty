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

#include "project.h"

#include <toml/toml.h>

#include <stdlib.h>
#include <sys/stat.h>
#ifdef N_WIN
#   include <direct.h>
#endif
#include <time.h>

#include "parser.h"
#include "util/str.h"

static string loadStringForKey(toml_table_t *table, conststr key, conststr defaultValue) {
    toml_datum_t datum = toml_string_in(table, key);
    if (datum.ok) {
        string value = str_new(datum.u.s, nullptr);
        free(datum.u.s);
        return value;
    }

    if (defaultValue != nullptr) {
        return str_new(defaultValue, nullptr);
    }

    return nullptr;
}

static bool loadBoolForKey(toml_table_t *table, conststr key, bool defaultValue) {
    toml_datum_t datum = toml_bool_in(table, key);
    if (datum.ok) {
        return datum.u.b;
    }

    return defaultValue;
}

ProjectInfo *loadProject() {
    ProjectInfo *info = (ProjectInfo*)malloc(sizeof(ProjectInfo));
    info->targetCount = 0;
    info->loaded = false;
    info->defaultTargetIdx = -1;
    
    info->config.verbosity = Debug; // TODO: Remove for release.

    FILE *fp = fopen(NIFTY_BUILD_FILE, "r");
    if (fp == nullptr) {
        println("Can't open '%s'.", NIFTY_BUILD_FILE);
        free(info);
        return nullptr;
    }

    char errbuf[200];
    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (conf == nullptr) {
        println("Can't parse build file.\n%s", errbuf);
        return info;
    }

    info->name = loadStringForKey(conf, "project", nullptr);
    info->config.disableColors = loadBoolForKey(conf, "disableColors", false);

    info->targets = (TargetInfo**)malloc(sizeof(TargetInfo*));
    info->targets[0] = (TargetInfo*)malloc(sizeof(TargetInfo));
    info->targetCount = 0;

    for (int i = 0;; ++i) {
        conststr key = toml_key_in(conf, i);
        if (!key) {
            break;
        }

        toml_table_t *tab = toml_table_in(conf, key);
        if (tab != nullptr) {
            ++info->targetCount;

            if (info->targetCount > 1) {
                info->targets = (TargetInfo**)realloc(info->targets, sizeof(TargetInfo*) * info->targetCount);
                info->targets[info->targetCount - 1] = (TargetInfo*)malloc(sizeof(TargetInfo));
            }
            TargetInfo *target = info->targets[info->targetCount - 1];

            target->targetName = str_new(key, nullptr);

            target->description = loadStringForKey(tab, "description", nullptr);
            target->outputName = loadStringForKey(tab, "outputName", target->targetName);
            target->entryPoint = loadStringForKey(tab, "entryPoint", nullptr);

            target->isDebugMode = loadBoolForKey(tab, "debug", false);
            target->isDefaultTarget = loadBoolForKey(tab, "default", false);
            if (target->isDefaultTarget && info->defaultTargetIdx < 0) {
                info->defaultTargetIdx = info->targetCount - 1;
            }
        }
    }

    if (info->defaultTargetIdx < 0) {
        info->defaultTargetIdx = 0;
        info->targets[0]->isDefaultTarget = true;
    }

    toml_free(conf);

    if (!info->config.disableColors) {
        info->config.disableColors = getenv("NIFTY_DISABLE_COLORS") != nullptr;
    }

    info->loaded = true;
    return info;
}

void freeProject(ProjectInfo *info) {
    if (info == nullptr) {
        return;
    }

    if (!info->loaded) {
        free(info);
        return;
    }

    for (int i = 0; i < info->targetCount; ++i) {
        TargetInfo *target = info->targets[i];
        str_delete(target->targetName);
        str_delete(target->outputName);
        str_delete(target->description);
        str_delete(target->entryPoint);
        free(target);
    }
    free(info->targets);
    str_delete(info->name);
    free(info);
}

TargetInfo *getTargetInfo(conststr targetName, ProjectInfo *info) {
    if (info->targetCount <= 0) {
        println("No targets found.");
        return nullptr;
    }
    
    if (str_empty(targetName)) {
        if (info->defaultTargetIdx > -1 && info->defaultTargetIdx < info->targetCount) {
            return info->targets[info->defaultTargetIdx];
        }
        
        for (int i = 0; i < info->targetCount; ++i) {
            TargetInfo *target = info->targets[i];
            if (target->isDefaultTarget) {
                return target;
            }
        }
        
        return info->targets[0];
    }
    
    for (int i = 0; i < info->targetCount; ++i) {
        TargetInfo *target = info->targets[i];
        if (str_eq(targetName, target->targetName)) {
            return target;
        }
    }
    
    println("Could not find target '%s'.", targetName);
    return nullptr;
}

void build(conststr targetName, ProjectInfo *info) {
    if (info == nullptr) {
        return;
    }
    
    TargetInfo *target = getTargetInfo(targetName, info);
    if (target == nullptr) {
        return;
    }
    
    if (info->config.verbosity >= Debug) {
        println("Building target '%s'.", target->targetName);
    }

    parseFile(target->entryPoint, &info->config);
}

void run(conststr targetName, ProjectInfo *info) {
    if (info == nullptr) {
        return;
    }
    
    TargetInfo *target = getTargetInfo(targetName, info);
    if (target == nullptr) {
        return;
    }

    build(targetName, info);
    
    if (info->config.verbosity >= Debug) {
        println("Running target '%s'.", target->targetName);
    }
}

void newProject(bool exists) {
    string answer = str_new_empty(100);

    if (exists) {
        println("A Nifty project already exists in this directory.");
        printf("Would you like to overwrite it? (no) > ");
        str_fgets(answer, 101, stdin, "no");
        str_tolower(answer);

        if (str_empty(answer) || str_eq2(answer, "no", "n")) {
            return;
        }

        if (!str_eq2(answer, "yes", "y")) {
            println("Unknown answer '%s'. Exiting.", answer);
            return;
        }

        dbln();
    }

    println("This utility will help you make a new Nifty project.");
    println("Run 'nifty help new' for more information.");
    dbln();

    CreateProjectInfo *info = (CreateProjectInfo*)malloc(sizeof(CreateProjectInfo));

    printf("Project name: (Untitled) > ");
    str_fgets(answer, 101, stdin, "Untitled");
    info->name = str_new(answer, nullptr);

    printf("Project version: (0.1.0) > ");
    str_fgets(answer, 101, stdin, "0.1.0");
    info->version = str_new(answer, nullptr);

    printf("Entry point: (main.nifty) > ");
    str_fgets(answer, 101, stdin, "main.nifty");
    info->entryPoint = str_new(answer, nullptr);

    printf("Author: > ");
    str_fgets(answer, 101, stdin, nullptr);
    info->author = str_new(answer, nullptr);

    printf("License: (zlib) > ");
    str_fgets(answer, 101, stdin, "zlib");
    info->license = str_new(answer, nullptr);

    const int width = 25;
    dbln();
    printStrsWithSpacer("Project name", '-', info->name, width);
    printStrsWithSpacer("Project version", '-', info->version, width);
    printStrsWithSpacer("Entry point", '-', info->entryPoint, width);
    if (!str_empty(info->author)) {
        printStrsWithSpacer("Author", '-', info->author, width);
    }
    printStrsWithSpacer("License", '-', info->license, width);

    dbln();
    printf("Is this ok? (yes) > ");
    str_fgets(answer, 4, stdin, "yes");
    str_tolower(answer);

    if (str_eq2(answer, "yes", "y")) {
        createProject(info);
    }

    free(info->name);
    free(info->version);
    free(info->entryPoint);
    free(info->author);
    free(info->license);
    free(info);
}

void createProject(CreateProjectInfo *info) {
    FILE *file = fopen(NIFTY_BUILD_FILE, "w");
    if (file == nullptr) {
        println("Could not open %s for writing. Exiting.", NIFTY_BUILD_FILE);
        return;
    }


    if (!str_empty(info->author)) {
        fprintf(file, "project = \"%s\"\n", info->name);
        fprintf(file, "author = \"%s\"\n\n", info->author);
    } else {
        fprintf(file, "project = \"%s\"\n\n", info->name);
    }

    fprintf(file, "[%s]\n", info->name);
    fprintf(file, "description = \"%s debug build.\"\n", info->name);
    fprintf(file, "outputName = \"%s\"\n", info->name);
    fprintf(file, "entryPoint = \"src/%s\"\n", info->entryPoint);
    fprintf(file, "debug = true\n");
    fprintf(file, "default = true\n");

    fprintf(file, "\n");

    fprintf(file, "[%s_release]\n", info->name);
    fprintf(file, "description = \"%s release build.\"\n", info->name);
    fprintf(file, "outputName = \"%s\"\n", info->name);
    fprintf(file, "entryPoint = \"src/%s\"\n", info->entryPoint);
    fprintf(file, "optimization = \"fast\"\n");

    fclose(file);

    struct stat st = {0};
    if (stat("src", &st)) {
#ifdef N_WIN
        if (_mkdir("src") != 0) {
#else
        if (mkdir("src", 0755) != 0) {
#endif
            println("Could not create src folder. Exiting.");
            return;
        }
    }

    string fileName = str_new_empty(100);
    sprintf(fileName, "src/%s", info->entryPoint);
    file = fopen(fileName, "w");
    if (file == nullptr) {
        println("Could not open src/%s for writing. Exiting.", info->entryPoint);
        return;
    }
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int year = tm.tm_year + 1900;
    
    fprintf(file, "/-\n");
    fprintf(file, " - %s\n", info->name);
    if (!str_empty(info->author)) {
        fprintf(file, " - Copyright (c) %d %s\n", year, info->author);
    }
    fprintf(file, "-/\n\n");
    fprintf(file, "#package %s\n\n", info->name); // TODO: Check for whitespace and convert to underscores.
    fprintf(file, "#using <fmt>\n\n");
    fprintf(file, "fn main() {\n");
    fprintf(file, "    println(\"Hullo!\")\n");
    fprintf(file, "}\n");

    fclose(file);
    
    // TODO: Should I ask to generate README?
    file = fopen("README.md", "w");
    fprintf(file, "# %s\n", info->name);
    fprintf(file, "TODO: Useful information here.\n");
    fclose(file);
    
    file = fopen("license.md", "w");
    fprintf(file, "# TODO\n");
    fclose(file);

    println("Created project %s.", info->name);
}

void listTargets(ProjectInfo *info) {
    if (info == nullptr) {
        return;
    }

    printf("Targets in project %s:\n", info->name);

    int longestTargetName = str_len(info->targets[0]->targetName);
    for (int i = 1; i < info->targetCount; ++i) {
        int len = str_len(info->targets[i]->targetName);
        if (len > longestTargetName) {
            longestTargetName = len;
        }
    }

    for (int i = 0; i < info->targetCount; ++i) {
        TargetInfo *target = info->targets[i];
        if (target->isDefaultTarget) {
            if (!info->config.disableColors) {
                printf(GREEN_COLOR "*");
            } else {
                printf("*");
            }
        } else {
            printf(" ");
        }
        
        int width = max(longestTargetName + 5, 25);
        printStrsWithSpacer(target->targetName, '-', target->description, width);
        if (target->isDefaultTarget && !info->config.disableColors) {
            printf(RESET_COLOR);
        }
    }
}
