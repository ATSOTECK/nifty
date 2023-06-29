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

#include "project.hpp"

#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegenC.hpp"

#include <iomanip>
#include <filesystem>
#include <fstream>
#define TOML_EXCEPTIONS 0
#include <toml.hpp>

void Project::create(const CreateProjectInfo &info) {
    std::ofstream buildFile;
    buildFile.open(NIFTY_BUILD_FILE, std::ios_base::out);

    if (!buildFile.is_open()) {
        verr("Could not open " << NIFTY_BUILD_FILE << " for writing!");
        exit(0);
    }
    
    buildFile << "project = \"" << info.name << "\"" << std::endl << std::endl;

    buildFile << "[" << info.name << "]" << std::endl;
    buildFile << "description = \"Debug Build\"" << std::endl;
    buildFile << "outputName = \"" << info.name << "\"" << std::endl;
    buildFile << "entryPoint = \"src/" << info.entryPoint << "\"" << std::endl;
    buildFile << "debug = true" << std::endl;
    buildFile << "default = true" << std::endl;
    
    
    buildFile << std::endl;
    
    buildFile << "[" << info.name << "_release" << "]" << std::endl;
    buildFile << "description = \"Release Build\"" << std::endl;
    buildFile << "outputName = \"" << info.name << "\"" << std::endl;
    buildFile << "entryPoint = \"src/" << info.entryPoint << "\"" << std::endl;
    buildFile << "optimization = \"fast\"" << std::endl;
    
    buildFile.close();
    
    std::fstream license;
    license.open("license.md", std::ios_base::out);
    
    time_t t = std::time(nullptr);
    struct tm time{};
#ifdef VWIN
    localtime_s(&time, &t);
#else
    localtime_r(&t, &time);
#endif
    int year = time.tm_year + 1900;
    
    //std::transform(info.license.begin(), info.license.end(), info.license.begin(), [](unsigned char c){return std::tolower(c);});
    let licenseName = String(info.license).toLower().stdString();
    if (licenseName == "zlib") {
        license << "# " << info.name << "\n"
                   "\n"
                   "Copyright (c) " << year << " " << info.author << "\n"
                   "\n"
                   "This software is provided 'as-is', without any express or implied\n"
                   "warranty. In no event will the authors be held liable for any damages\n"
                   "arising from the use of this software.\n"
                   "\n"
                   "Permission is granted to anyone to use this software for any purpose,\n"
                   "including commercial applications, and to alter it and redistribute it\n"
                   "freely, subject to the following restrictions:\n"
                   "\n"
                   "1. The origin of this software must not be misrepresented; you must not\n"
                   "   claim that you wrote the original software. If you use this software\n"
                   "   in a product, an acknowledgment in the product documentation would be\n"
                   "   appreciated but is not required.\n"
                   "2. Altered source versions must be plainly marked as such, and must not be\n"
                   "   misrepresented as being the original software.\n"
                   "3. This notice may not be removed or altered from any source distribution.";
    } else if (licenseName == "mit") {
        license << "# " << info.name << "\n"
                   "\n"
                   "Copyright " << year << " " << info.author << "\n"
                   "\n"
                   "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
                   "of this software and associated documentation files (the \"Software\"), to deal\n"
                   "in the Software without restriction, including without limitation the rights\n"
                   "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies\n"
                   "of the Software, and to permit persons to whom the Software is furnished to do so,\n"
                   "subject to the following conditions:\n"
                   "\n"
                   "The above copyright notice and this permission notice shall be included in all\n"
                   "copies or substantial portions of the Software.\n"
                   "\n"
                   "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
                   "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
                   "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
                   "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
                   "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING\n"
                   "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS\n"
                   "IN THE SOFTWARE.";
    } else {
        license << "# " << info.name << "\n"
                   "\n"
                   "Copyright (c) YEAR " << info.author << "\n"
                   "\n"
                   "" << info.license;
    }
    
    license.close();
    
    std::filesystem::create_directories("./src");
    
    std::fstream entry;
    entry.open("./src/" + info.entryPoint, std::ios_base::out);
    
    entry << "/-" << std::endl;
    entry << " - " << info.name << std::endl;
    entry << " - Copyright (c) " << year << " " << info.author << std::endl;
    entry << " -/" << std::endl << std::endl;
    
    entry << "package " << info.name << std::endl << std::endl;
    entry << "using <fmt>" << std::endl << std::endl;
    
    entry << "fn main() {" << std::endl;
    entry << "    println(\"Hullo!\")" << std::endl;
    entry << "}" << std::endl;
    
    entry.close();
    
    db("Created project '" << info.name << "'.\nExiting.");
}

bool Project::build(const String &targetName) {
    let projectInfo = Project::getProjectInfo();
    if (projectInfo.projectName.empty()) {
        return false;
    }
    
    String entryPoint = "";
    if (!targetName.empty()) {
        for (const let& target : projectInfo.targets) {
            if (target.targetName == targetName) {
                entryPoint = target.entryPoint;
            }
        }
        
        if (entryPoint.empty()) {
            std::cout << "Build error: No such target '" << targetName << "' in " << NIFTY_BUILD_FILE << ".\n";
            return false;
        }
    } else {
        entryPoint = projectInfo.targets[projectInfo.defaultTargetIdx].entryPoint;
    }
    
    Lexer lexer(entryPoint);
    Parser parser = Parser(&lexer);
    Nodes ast = parser.parse();
    CodegenC codegenC;
    codegenC.generate(ast);
    if (parser.finishedWithErrors()) {
        return false;
    }
    
    return true;
}

void Project::run(const String &targetName) {
    bool success = build(targetName);
    if (!success) {
        return;
    }
    
    db("run");
    // TODO: Run.
}

void Project::listTargets() {
    let projectInfo = Project::getProjectInfo();
    if (projectInfo.projectName.empty()) {
        return;
    }
    
    let maxWidth = 0;
    for (const let& t : projectInfo.targets) {
        if (t.targetName.length() > maxWidth) {
            maxWidth = (int)t.targetName.length();
        }
    }
    
    String defaultTarget;
    std::cout << "Targets in project " << projectInfo.projectName << ":\n";
    for (const let& t : projectInfo.targets) {
        if (t.description.empty()) {
            if (t.isDefaltTarget) {
                db(" *" << t.targetName);
                defaultTarget = t.targetName;
            } else {
                db("  " << t.targetName);
            }
        } else {
            if (t.isDefaltTarget) {
                db(" *" << std::left << std::setfill('-') << std::setw(maxWidth + 5) << t.targetName + " " << " "
                        << t.description);
                defaultTarget = t.targetName;
            } else {
                db("  " << std::left << std::setfill('-') << std::setw(maxWidth + 5) << t.targetName + " " << " "
                        << t.description);
            }
        }
    }
    
    std::cout << "\nTarget '" << defaultTarget << "' is marked as default.\n";
}

void Project::print(const CreateProjectInfo &info) {
    db(std::setfill('-') << std::left << std::setw(30));
    db(std::setw(30) << "Project name "    << " " << info.name);
    db(std::setw(30) << "Project version " << " " << info.version);
    db(std::setw(30) << "Entry point "     << " " << info.entryPoint);
    if (!info.author.empty()) {
        db(std::setw(30) << "Author " << " " << info.author);
    }
    db(std::setw(30) << "License "         << " " << info.license);
}

ProjectInfo Project::getProjectInfo() {
    toml::parse_result result = toml::parse_file(NIFTY_BUILD_FILE);
    if (!result) {
        std::cerr << "Parsing " << NIFTY_BUILD_FILE << " failed:\n" << result.error() << std::endl;
    }
    
    toml::table table = result.table();
    ProjectInfo projectInfo{};
    bool defaultFound = false;
    String defaultTargetName;
    
    int idx = 0;
    for (let&& [k, v] : table) {
        const let type = v.type();
        if (type == toml::node_type::table) {
            TargetInfo targetInfo{};
            
            targetInfo.targetName = k.data();
            targetInfo.outputName = v.at_path("outputName").value_or("");
            targetInfo.entryPoint = v.at_path("entryPoint").value_or("");
            targetInfo.optimization = v.at_path("optimization").value_or("none");
            targetInfo.description = v.at_path("description").value_or("");
            targetInfo.isDebugMode = v.at_path("debug").value_or(false);
            targetInfo.isDefaltTarget = v.at_path("default").value_or(false);
            targetInfo.noMain = v.at_path("noMain").value_or(false);
            targetInfo.cmdOnly = v.at_path("cmdOnly").value_or(false);
            targetInfo.noDepreciated = v.at_path("noDepreciated").value_or(false);
            targetInfo.noGoto = v.at_path("noGoto").value_or(true);
            targetInfo.boundsChecks = v.at_path("boundsChecks").value_or("always");
            targetInfo.errorOnWarn = v.at_path("errorOnWarn").value_or(false);
            targetInfo.noWarn = v.at_path("noWarn").value_or(false);
            targetInfo.disallowNull = v.at_path("disallowNull").value_or(false);
            targetInfo.deprecatedAfterWarnWithin = v.at_path("deprecatedAfterWarnWithin").value_or(0);
            targetInfo.noUnusedOnErrorReturn = v.at_path("noUnusedOnErrorReturn").value_or(false);
            targetInfo.fastMath = v.at_path("fastMath").value_or(false);
            targetInfo.finiteMathOnly = v.at_path("finiteMathOnly").value_or(false);
            targetInfo.associativeMath = v.at_path("associativeMath").value_or(false);
            targetInfo.unsafeMathOptimization = v.at_path("unsafeMathOptimization").value_or(false);
            
            
            if (targetInfo.isDefaltTarget && !defaultFound) {
                defaultFound = true;
                defaultTargetName = targetInfo.targetName;
                projectInfo.defaultTargetIdx = idx;
            } else if (targetInfo.isDefaltTarget && defaultFound) {
                std::cerr << "Config Error: Can only have one default target." << std::endl;
                std::cerr << "Target '" << defaultTargetName << "' already set as default. Target '" << k
                          << "' can't also be set as default." << std::endl;
                std::cerr << "Exiting." << std::endl;
                std::exit(1);
            }
            
            projectInfo.targets.push_back(targetInfo);
            ++idx;
        } else if (type == toml::node_type::string) {
            if (k == "project") {
                projectInfo.projectName = v.value_or("");
            }
        }
    }
    
    if (!defaultFound) {
        std::cout << "Config Warning: Default target not found. Setting '" << projectInfo.targets[0].targetName << "' as default.\n\n";
        projectInfo.targets[0].isDefaltTarget = true;
        projectInfo.defaultTargetIdx = 0;
    }
    
    if (verifyProjectInfo(projectInfo)) {
        return projectInfo;
    }
    
    return ProjectInfo{};
}

bool Project::verifyProjectInfo(const ProjectInfo &info) {
    #define invalid(v, k, t) db("Config error: Invalid value '" << (v) << "' for " << (k) << " in target '" << (t) << "'.")
    
    if (info.projectName.empty()) {
        db("Config error: Nifty projects must have a name.");
        db("To set the project name use:");
        db("project = \"name\"");
        db("The project name must be set before the targets.");
        return false;
    }
    
    for (const let& target : info.targets) {
        if (target.optimization != "none" && target.optimization != "fast" && target.optimization != "size"
            && target.optimization != "debug") {
            invalid(target.optimization, "optimization", target.targetName);
            db("Valid options are none, fast, size, debug.");
            return false;
        }
        
        if (target.boundsChecks != "always" && target.boundsChecks != "debug" && target.boundsChecks != "never") {
            invalid(target.boundsChecks, "boundsChecks", target.targetName);
            db("Valid options are always, debug, never.");
            return false;
        }
        
        if (!target.cmdOnly && target.entryPoint.empty()) {
            db("Entry point must be set for target '" << "'.");
            return false;
        }
    }

    #undef invalid
    
    return true;
}
