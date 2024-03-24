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
    string name;
    string version;
    string entryPoint;
    string author;
    string license;
} CreateProjectInfo;

typedef struct {
    string targetName;
    string outputName;
    string entryPoint;
    bool isDebugMode;
    string optimization;
    string description;
    bool isDefaultTarget;
    bool noMain;
    bool cmdOnly;
    bool noDepreciated;
    bool noGoto;
    string boundsChecks;
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
    string name;
    int defaultTargetIdx;
    int targetCount;
    TargetInfo **targets;
    
    CompilerConfig config;
} ProjectInfo;

ProjectInfo *loadProject();
void freeProject(ProjectInfo *info);

TargetInfo *getTargetInfo(conststr targetName, ProjectInfo *info);

void build(conststr targetName, ProjectInfo *info);
void run(conststr targetName, ProjectInfo *info);
void newProject(bool exists);
void createProject(CreateProjectInfo *info);

void listTargets(ProjectInfo *info);
void printProject(CreateProjectInfo *info);

ProjectInfo getProjectInfo();
bool verifyProjectInfo(CreateProjectInfo info);

#endif //NIFTY_PROJECT_H
