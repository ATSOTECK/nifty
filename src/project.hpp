/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2022 - 2023 Skyler Burwell
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

#ifndef __NIFTY_PROJECT_HPP__
#define __NIFTY_PROJECT_HPP__

#include "util/str.hpp"

#include <string>
#include <vector>

struct CreateProjectInfo {
    std::string name;
    std::string version;
    std::string entryPoint;
    std::string author;
    std::string license;
};

struct TargetInfo {
    String targetName;
    String outputName;
    String entryPoint;
    bool isDebugMode;
    String optimization;
    String description;
    bool isDefaltTarget;
    bool noMain;
    bool cmdOnly;
    bool noDepreciated;
    bool noGoto;
    String boundsChecks;
    bool errorOnWarn;
    bool noWarn;
    bool disallowNull;
    int deprecatedAfterWarnWithin;
    bool noUnusedOnErrorReturn;
    bool fastMath;
    bool finiteMathOnly;
    bool associativeMath;
    bool unsafeMathOptimization;
    
    // Strings children;
    // Strings defines;
    
    // add executeBefore, executeAfter
    
    // Strings includeDirs;
    // Strings linkDirs;
};

struct ProjectInfo {
    String projectName;
    int defaultTargetIdx;
    std::vector<TargetInfo> targets;
};



class Project {
public:
    static void create(const CreateProjectInfo &info);
    static bool build(const String &targetName);
    static void run(const String &targetName);
    static void listTargets();
    static void print(const CreateProjectInfo &info);
    
private:
    static ProjectInfo getProjectInfo();
    static bool verifyProjectInfo(const ProjectInfo &info);
    static TargetInfo getTargetInfo(const String &targetName);
    
    static void callClang(const String &exeName);
    static void callGeneratedExe(const String &exeName);
};


#endif //__NIFTY_PROJECT_HPP__
