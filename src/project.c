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

ProjectInfo loadProject() {
    ProjectInfo info;
    info.targetCount = 0;
    info.loaded = false;
    info.defaultTargetIdx = -1;

    FILE *fp = fopen(NIFTY_BUILD_FILE, "r");
    if (fp == nullptr) {
        printf("cant open build\n");
        return info;
    }

    char errbuf[200];
    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (conf == nullptr) {
        printf("cant parse build\n");
        return info;
    }

    toml_datum_t projectName = toml_string_in(conf, "project");
    if (projectName.ok) {
        info.name = str_new(projectName.u.s, nullptr);
        free(projectName.u.s);
    }

    info.targets = (TargetInfo*)malloc(sizeof(TargetInfo));
    info.targetCount = 0;

    for (int i = 0;; ++i) {
        conststr key = toml_key_in(conf, i);
        if (!key) {
            break;
        }

        toml_table_t *tab = toml_table_in(conf, key);
        if (tab != nullptr) {
            ++info.targetCount;

            if (info.targetCount > 1) {
                info.targets = (TargetInfo*)realloc(info.targets, sizeof(TargetInfo) * info.targetCount);
            }
            TargetInfo *target = &info.targets[info.targetCount - 1];

            target->targetName = str_new(key, nullptr);

            target->description = loadStringForKey(tab, "description", nullptr);
            target->outputName = loadStringForKey(tab, "outputName", target->targetName);
            target->entryPoint = loadStringForKey(tab, "entryPoint", nullptr);

            target->isDebugMode = loadBoolForKey(tab, "debug", false);
            target->isDefaltTarget = loadBoolForKey(tab, "default", false);
            if (target->isDefaltTarget && info.defaultTargetIdx < 0) {
                info.defaultTargetIdx = info.targetCount - 1;
            }
        }
    }

    toml_free(conf);

    for (int i = 0; i < info.targetCount; ++i) {
        TargetInfo target = info.targets[i];
        printf("[%s]\n", target.targetName);
    }
    printf("\n\n");

    info.loaded = true;
    return info;
}

void build(conststr target, ProjectInfo info) {
    printf("build\n");
    if (target != nullptr) {
        printf("target: %s\n", target);
    }
}

void run(conststr target, ProjectInfo info) {
    build(target, info);
    printf("run\n");
}

void newProject() {
    printf("new\n");
}
