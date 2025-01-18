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

#ifndef NIFTY_PROJECT_H
#define NIFTY_PROJECT_H

#include "common.h"
#include "util/str.h"

typedef struct {
    char *name;
    char *version;
    char *entryPoint;
    char *author;
    char *license;
} CreateProjectInfo;

typedef struct {
    char *targetName;
    char *outputName;
    char *entryPoint;
    bool isDebugMode;
    char *optimization;
    char *description;
    bool isDefaultTarget;
    bool noMain;
    bool cmdOnly;
    bool noDepreciated;
    bool noGoto;
    char *boundsChecks;
    bool errorOnWarn;
    bool noWarn;
    bool disallowNull;
    int deprecatedAfterWarnWithin;
    bool noUnusedOnErrorReturn;
    bool fastMath;
    bool finiteMathOnly;
    bool associativeMath;
    bool unsafeMathOptimization;
} TargetInfo;

typedef struct {
    bool loaded;
    char *name;
    int defaultTargetIdx;
    int targetCount;
    TargetInfo **targets;
    
    CompilerConfig config;
    bool buildFailed;
} ProjectInfo;

ProjectInfo *loadProject();
void freeProject(ProjectInfo *info);

TargetInfo *getTargetInfo(const char *targetName, ProjectInfo *info);

void build(const char *targetName, ProjectInfo *info);
void run(const char *targetName, ProjectInfo *info);
void newProject(bool exists);
void createProject(const CreateProjectInfo *info);

void listTargets(const ProjectInfo *info);
void printProject(CreateProjectInfo *info);

bool verifyProjectInfo(CreateProjectInfo *info);

#endif //NIFTY_PROJECT_H
