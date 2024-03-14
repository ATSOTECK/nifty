//
// Created by Skyler on 3/6/24.
//

#include "project.h"

#include <toml/toml.h>

#include <stdlib.h>

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
        printf("cant parse build\n");
        return info;
    }

    info->name = loadStringForKey(conf, "project", nullptr);
    info->disableColors = loadBoolForKey(conf, "disableColors", false);

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

    if (!info->disableColors) {
        info->disableColors = getenv("NIFTY_DISABLE_COLORS") != nullptr;
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

void build(conststr target, ProjectInfo *info) {
    if (info == nullptr) {
        return;
    }

    printf("build\n");
    if (target != nullptr) {
        printf("target: %s\n", target);
    }
}

void run(conststr target, ProjectInfo *info) {
    if (info == nullptr) {
        return;
    }

    build(target, info);
    printf("run\n");
}

void newProject() {
    printf("new\n");
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
            if (!info->disableColors) {
                printf(GREEN "*");
            } else {
                printf("*");
            }
        } else {
            printf(" ");
        }
        printStrsWithSpacer(target->targetName, '-', target->description, longestTargetName + 5);
        if (target->isDefaultTarget && !info->disableColors) {
            printf(RESET_COLOR);
        }
    }
}
