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

#include <iomanip>
#include <filesystem>
#include <fstream>
#define TOML_EXCEPTIONS 0
#include <toml.hpp>

void Project::create(CreateProjectInfo &info) {
    std::ofstream buildfile;
    buildfile.open(NIFTY_BUILD_FILE, std::ios_base::out);

    if (!buildfile.is_open()) {
        verr("Could not open " << NIFTY_BUILD_FILE << " for writing!");
        exit(0);
    }
    
    buildfile << "project = \"" << info.name << "\"" << std::endl << std::endl;

    buildfile << "[" << info.name << "]" << std::endl;
    buildfile << "description = \"Debug Build\"" << std::endl;
    buildfile << "outputName = \"" << info.name << "\"" << std::endl;
    buildfile << "entryPoint = \"" << info.entryPoint << "\"" << std::endl;
    buildfile << "debug = true" << std::endl;
    buildfile << "default = true" << std::endl;
    
    
    buildfile << std::endl;
    
    buildfile << "[" << info.name << "_release" << "]" << std::endl;
    buildfile << "description = \"Release Build\"" << std::endl;
    buildfile << "outputName = \"" << info.name << "\"" << std::endl;
    buildfile << "entryPoint = \"" << info.entryPoint << "\"" << std::endl;
    buildfile << "optimization = \"fast\"" << std::endl;
    
    buildfile.close();
    
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
    info.license = String(info.license).toLower().stdString();
    if (info.license == "zlib") {
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
    } else if (info.license == "mit") {
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

void Project::build(const String &target) {
    let projectInfo = Project::getProjectInfo();
    std::cerr << "build\n";
}

void Project::listTargets() {
    let projectInfo = Project::getProjectInfo();
    
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
    
    for (let&& [k, v] : table) {
        const let type = v.type();
        if (type == toml::node_type::table) {
            TargetInfo targetInfo{};
            
            // Required.
            targetInfo.targetName = k.data();
            targetInfo.outputName = v.at_path("outputName").value_or("");
            targetInfo.entryPoint = v.at_path("entryPoint").value_or("");
            
            // Optional.
            targetInfo.description = v.at_path("description").value_or("");
            targetInfo.isDebugMode = v.at_path("debug").value_or(false);
            targetInfo.isDefaltTarget = v.at_path("default").value_or(false);
            
            if (targetInfo.isDefaltTarget && !defaultFound) {
                defaultFound = true;
                defaultTargetName = targetInfo.targetName;
            } else if (targetInfo.isDefaltTarget && defaultFound) {
                std::cerr << "Config Error: Can only have one default target." << std::endl;
                std::cerr << "Target '" << defaultTargetName << "' already set as default. Target '" << k
                          << "' can't also be set as default." << std::endl;
                std::cerr << "Exiting." << std::endl;
                std::exit(1);
            }
            
            projectInfo.targets.push_back(targetInfo);
        } else if (type == toml::node_type::string) {
            if (k == "project") {
                projectInfo.projectName = v.value_or("");
            }
        }
    }
    
    if (!defaultFound) {
        std::cout << "Config Warning: Default target not found. Setting '" << projectInfo.targets[0].targetName << "' as default.\n\n";
        projectInfo.targets[0].isDefaltTarget = true;
    }
    
    return projectInfo;
}
