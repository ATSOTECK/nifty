//
// Created by Skyler on 2/1/24.
//

#ifndef NIFTY_PROJECT_H
#define NIFTY_PROJECT_H

#include "common.h"

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
    
    bool disableColors;
} ProjectInfo;

ProjectInfo loadProject();

void build(conststr target, ProjectInfo info);
void run(conststr target, ProjectInfo info);
void newProject();

void createProject(CreateProjectInfo info);
bool buildTarget(string target);
void runTarget(string target);
void listTargets(ProjectInfo info);
void printProject(CreateProjectInfo info);

ProjectInfo getProjectInfo();
bool verifyProjectInfo(CreateProjectInfo info);
TargetInfo getTargetInfo(string targetName);

#endif //NIFTY_PROJECT_H
