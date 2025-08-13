//
// Created by Skyler Burwell on 6/5/25.
//

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "common.hpp"

#include <string>
#include <vector>

typedef struct {
    std::string name;
    std::string version;
    std::string entryPoint;
    std::string author;
    std::string license;
    bool noGit;
} CreateProjectInfo;

struct TargetInfo {
    std::string targetName;
    std::string outputName;
    std::string entryPoint;
    bool isDebugMode;
    std::string optimization;
    std::string description;
    bool isDefaultTarget;
    bool noMain;
    bool cmdOnly;
    bool noDepreciated;
    bool noGoto;
    std::string boundsChecks;
    bool errorOnWarn;
    bool noWarn;
    bool disallowNull;
    int deprecatedAfterWarnWithin;
    bool noUnusedOnErrorReturn;
    bool fastMath;
    bool finiteMathOnly;
    bool associativeMath;
    bool unsafeMathOptimization;
};

struct ProjectInfo {
    bool loaded;
    std::string name;
    int defaultTargetIdx;
    int targetCount;
    std::vector<TargetInfo*> targets;

    CompilerConfig config;
    bool buildFailed;
};

ProjectInfo loadProject();

void build(const std::string &target_name, ProjectInfo &info);
void run(const std::string &target_name, ProjectInfo &info);
void new_project(bool exists, bool noGit);
void create_basic_project(const std::string &name, bool noGit);

void list_targets(const ProjectInfo &info);
void list_tasks(const ProjectInfo &info);

#endif //PROJECT_HPP
