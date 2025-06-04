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

#include <git2.h>
#include <toml/toml.h>

#include <stdlib.h>
#include <sys/stat.h>
#ifdef N_WIN
#   include <direct.h>
#endif
#include <time.h>

#include "parser.h"
#include "util/str.h"

#ifdef N_WIN
#   define MKDIR_CHECK() _mkdir(folder)
#else
#   define MKDIR_CHECK() mkdir(folder, 0755)
#endif

static bool createFolder(const char *folder) {
    struct stat st = {0};
    if (stat(folder, &st)) {
        if (MKDIR_CHECK() != 0) {
            println("Could not create %s folder. Exiting.", folder);
            return false;
        }
    }

    return true;
}

static bool createRepo(const CreateProjectInfo *info) {
    git_libgit2_init();

    git_repository *repo = nullptr;
    char *repoName = str_new_fmt(nullptr, "%s", info->name);
    git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
    opts.description = str_new_fmt(nullptr, "%s - Nifty Project", info->name);
    opts.initial_head = "main";

    const int error = git_repository_init_ext(&repo, repoName, &opts);
    str_delete(repoName);

    if (error != 0) {
        println("Could not create repo for project %s. Exiting.", info->name);
        git_libgit2_shutdown();
        return false;
    }

    git_libgit2_shutdown();

    char *gitignore = str_new_fmt(nullptr, "%s/.gitignore", info->name);
    FILE *file = fopen(gitignore, "w");
    if (file == nullptr) {
        println("Could not open .gitignore for writing. Exiting.");
        return false;
    }
    str_delete(gitignore);

    fprintf(file, "# Build\n");
    fprintf(file, "build-debug\n");
    fprintf(file, "build-release\n");

    fprintf(file, "\n# OS\n");
    fprintf(file, ".DS_Store\n");

    fclose(file);

    return true;
}

static char *loadStringForKey(const toml_table_t *table, const char *key, const char *defaultValue) {
    const toml_datum_t datum = toml_string_in(table, key);
    if (datum.ok) {
        char *value = str_new(datum.u.s, nullptr);
        free(datum.u.s);
        return value;
    }

    if (defaultValue != nullptr) {
        return str_new(defaultValue, nullptr);
    }

    return nullptr;
}

static bool loadBoolForKey(const toml_table_t *table, const char *key, const bool defaultValue) {
    const toml_datum_t datum = toml_bool_in(table, key);
    if (datum.ok) {
        return datum.u.b;
    }

    return defaultValue;
}

static void projectError(const ProjectInfo *info) {
    setTextColor(&info->config, ERROR_COLOR);
    printf("Project error:");
    setTextColor(&info->config, RESET_COLOR);
    printf(" ");
}

ProjectInfo *loadProject() {
    ProjectInfo *info = (ProjectInfo*)malloc(sizeof(ProjectInfo));
    info->targetCount = 0;
    info->loaded = false;
    info->defaultTargetIdx = -1;
    info->buildFailed = false;
    
    info->config.verbosity = Debug; // TODO: Remove for release.
    info->config.disableColors = getenv("NIFTY_DISABLE_COLORS") != nullptr;

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
        projectError(info);
        println("Can't parse build file.\n%s", errbuf);
        return info;
    }

    info->name = loadStringForKey(conf, "project", nullptr);
    info->config.disableColors = loadBoolForKey(conf, "disableColors", false);

    info->targets = (TargetInfo**)malloc(sizeof(TargetInfo*));
    info->targets[0] = (TargetInfo*)malloc(sizeof(TargetInfo));
    info->targetCount = 0;

    for (int i = 0;; ++i) {
        const char *key = toml_key_in(conf, i);
        if (!key) {
            break;
        }

        const toml_table_t *tab = toml_table_in(conf, key);
        if (tab != nullptr) {
            ++info->targetCount;

            if (info->targetCount > 1) {
                TargetInfo **reallocTargets = (TargetInfo**)realloc(info->targets, sizeof(TargetInfo*) * info->targetCount);
                if (reallocTargets == nullptr) {
                    freeProject(info);
                    println("Can't reallocate targets.");

                    return nullptr;
                }

                info->targets = reallocTargets;
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

TargetInfo *getTargetInfo(const char *targetName, const ProjectInfo *info) {
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

    projectError(info);
    printf("Could not find target ");
    setTextColor(&info->config, HIGHLIGHT_COLOR);
    printf("%s", targetName);
    setTextColor(&info->config, RESET_COLOR);
    println(".");

    return nullptr;
}

void build(const char *targetName, ProjectInfo *info) {
    if (info == nullptr) {
        println("No project found, nothing to build.");
        return;
    }

    const TargetInfo *target = getTargetInfo(targetName, info);
    if (target == nullptr) {
        return;
    }
    
    if (info->config.verbosity >= Debug) {
        printf("Building target ");
        setTextColor(&info->config, HIGHLIGHT_COLOR);
        printf("%s", target->targetName);
        setTextColor(&info->config, RESET_COLOR);
        println(".");
    }

    ParseResults *results = parseFile(target->entryPoint, &info->config);
    if (results->errorCount > 0) {
        info->buildFailed = true;

        if (results->errorCount > 1) {
            println("\nBuild finished with %d errors.", results->errorCount);
        } else {
            println("\nBuild finished with an error.");
        }
    }

    freeParseResults(results);
}

void run(const char *targetName, ProjectInfo *info) {
    if (info == nullptr) {
        println("No project found, nothing to run.");
        return;
    }

    const TargetInfo *target = getTargetInfo(targetName, info);
    if (target == nullptr) {
        return;
    }

    build(targetName, info);

    if (info->buildFailed) {
        return;
    }
    
    if (info->config.verbosity >= Debug) {
        printf("Running target ");
        setTextColor(&info->config, HIGHLIGHT_COLOR);
        printf("%s", target->targetName);
        setTextColor(&info->config, RESET_COLOR);
        println(".");
    }
}

void newProject(const bool exists, const bool noGit) {
    char *answer = str_new_empty(100);

    if (exists) {
        println("A Nifty project already exists in this directory. Exiting.");
        return;
    }

    println("This utility will help you make a new Nifty project.");
    println("Run 'nifty help new' for more information.");
    dbln();

    CreateProjectInfo info;
    info.noGit = noGit;

    printf("Project name: (Untitled) > ");
    str_fgets(answer, 101, stdin, "Untitled");
    info.name = str_new(answer, nullptr);

    printf("Project version: (0.1.0) > ");
    str_fgets(answer, 101, stdin, "0.1.0");
    info.version = str_new(answer, nullptr);

    printf("Entry point: (main.nifty) > ");
    str_fgets(answer, 101, stdin, "main.nifty");
    info.entryPoint = str_new(answer, nullptr);

    printf("Author: > ");
    str_fgets(answer, 101, stdin, nullptr);
    info.author = str_new(answer, nullptr);

    printf("License: (zlib) > ");
    str_fgets(answer, 101, stdin, "zlib");
    info.license = str_new(answer, nullptr);

    const int width = 25;
    dbln();
    printStrsWithSpacer("Project name", '-', info.name, width);
    printStrsWithSpacer("Project version", '-', info.version, width);
    printStrsWithSpacer("Entry point", '-', info.entryPoint, width);
    if (!str_empty(info.author)) {
        printStrsWithSpacer("Author", '-', info.author, width);
    }
    printStrsWithSpacer("License", '-', info.license, width);

    dbln();
    printf("Is this ok? (yes) > ");
    str_fgets(answer, 4, stdin, "yes");
    str_tolower(answer);

    if (str_eq2(answer, "yes", "y")) {
        createProject(&info);
    } else {
        println("Did not create project %s. Exiting.", info.name);
    }

    str_delete(info.name);
    str_delete(info.version);
    str_delete(info.entryPoint);
    str_delete(info.author);
    str_delete(info.license);
}

void createProject(const CreateProjectInfo *info) {
    if (!createFolder(info->name)) {
        return;
    }

    char *buildFile = str_new_fmt(nullptr, "%s/%s", info->name, NIFTY_BUILD_FILE);
    FILE *file = fopen(buildFile, "w");
    if (file == nullptr) {
        println("Could not open %s for writing. Exiting.", NIFTY_BUILD_FILE);
        return;
    }
    str_delete(buildFile);

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

    char *srcFolder = str_new_fmt(nullptr, "%s/src", info->name);
    if (!createFolder(srcFolder)) {
        return;
    }
    str_delete(srcFolder);

    char *debugFolder = str_new_fmt(nullptr, "%s/build-debug", info->name);
    if (!createFolder(debugFolder)) {
        return;
    }
    str_delete(debugFolder);

    char *filename = str_new_fmt(nullptr, "%s/src/%s", info->name, info->entryPoint);
    file = fopen(filename, "w");
    if (file == nullptr) {
        println("Could not open src/%s for writing. Exiting.", info->entryPoint);
        return;
    }
    str_delete(filename);

    const time_t t = time(NULL);
    const struct tm tm = *localtime(&t);
    const int year = tm.tm_year + 1900;
    
    fprintf(file, "/-\n");
    fprintf(file, " - %s\n", info->name);
    if (!str_empty(info->author)) {
        fprintf(file, " - Copyright (c) %d %s\n", year, info->author);
    }
    fprintf(file, " -/\n\n");
    fprintf(file, "namespace main\n\n");
    fprintf(file, "using fmt\n\n");
    fprintf(file, "fn main() {\n");
    fprintf(file, "    println(\"Hullo!\")\n");
    fprintf(file, "}\n");

    fclose(file);
    
    // TODO: Should I ask to generate README?
    char *readme = str_new_fmt(nullptr, "%s/README.md", info->name);
    file = fopen(readme, "w");
    if (file == nullptr) {
        println("Could not open README.md for writing. Exiting.");
    }
    str_delete(readme);

    fprintf(file, "# %s\n", info->name);
    fprintf(file, "TODO: Useful information here.\n");
    fclose(file);

    char *license = str_new_fmt(nullptr, "%s/license.md", info->name);
    file = fopen(license, "w");
    if (file == nullptr) {
        println("Could not open license.md for writing. Exiting.");
    }
    str_delete(license);

    fprintf(file, "# TODO\n");
    fclose(file);

    if (!info->noGit) {
        createRepo(info);
    }

    println("Created project %s.", info->name);
}

void listTargets(const ProjectInfo *info) {
    if (info == nullptr) {
        println("No project found, no targets to list.");
        return;
    }

    printf("Targets in project %s:\n", info->name);

    int longestTargetName = str_len(info->targets[0]->targetName);
    for (int i = 1; i < info->targetCount; ++i) {
        const int len = str_len(info->targets[i]->targetName);
        if (len > longestTargetName) {
            longestTargetName = len;
        }
    }

    for (int i = 0; i < info->targetCount; ++i) {
        const TargetInfo *target = info->targets[i];
        if (target->isDefaultTarget) {
            setTextColor(&info->config, LINE_COLOR);
            printf("*");
        } else {
            printf(" ");
        }

        const int width = max(longestTargetName + 5, 25);
        printStrsWithSpacer(target->targetName, '-', target->description, width);
        if (target->isDefaultTarget) {
            setTextColor(&info->config, RESET_COLOR);
        }
    }
}
