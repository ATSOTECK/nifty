//
// Created by Skyler Burwell on 6/5/25.
//

#include "project.hpp"

#include "compiler.hpp"
#include "util.hpp"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>
#include <git2.h>
#include <fmt/color.h>

#include <fstream>
#include <iostream>

ProjectInfo loadProject() {
    ProjectInfo info{};
    info.targetCount = 0;
    info.loaded = false;
    info.defaultTargetIdx = -1;
    info.buildFailed = false;

    info.config.verbosity = Verbosity::Debug; // TODO: Remove for release.
    info.config.disableColors = getenv("NIFTY_DISABLE_COLORS") != nullptr;

    const auto config = toml::parse_file(NIFTY_BUILD_FILE);
    if (!config) {
        std::string err(config.error().description());
        println("Parsing {} failed with error:\n{}", NIFTY_BUILD_FILE, err);
        return ProjectInfo{};
    }

    info.name = config["project"].value_or(NIFTY_DEFAULT_PROJECT);
    info.config.disableColors = config["disable_colors"].value_or(false);

    for (const auto [first, second] : config) {
        if (!second.is_table()) {
            continue;
        }

        const auto table = *second.as_table();

        TargetInfo *target = new TargetInfo{};
        target->targetName = first.data();
        target->description = table["description"].value_or("");
        target->outputName = table["output_name"].value_or(target->targetName);
        target->entryPoint = table["entry_point"].value_or("");
        target->isDebugMode = table["debug"].value_or(false);
        target->isDefaultTarget = table["default"].value_or(false);
        if (target->isDefaultTarget && info.defaultTargetIdx < 0) {
            info.defaultTargetIdx = info.targetCount - 1;
        }

        info.targets.push_back(target);
        ++info.targetCount;
    }

    if (info.defaultTargetIdx < 0) {
        info.defaultTargetIdx = 0;
        info.targets[0]->isDefaultTarget = true;
    }

    info.loaded = true;
    return info;
}

static void project_error(const ProjectInfo &info) {
    if (info.config.disableColors) {
        print("Project error: ");
    } else {
        print("{}Project error:{} ", ERROR_COLOR, RESET_COLOR);
    }
}

static TargetInfo *get_target_info(const std::string &target_name, const ProjectInfo &info) {
    if (info.targetCount <= 0) {
        println("No targets found. Exiting.");
        return nullptr;
    }

    if (target_name.empty()) {
        for (auto *target : info.targets) {
            if (target->isDefaultTarget) {
                return target;
            }
        }

        return info.targets[0];
    }

    for (auto *target : info.targets) {
        if (target->targetName == target_name) {
            return target;
        }
    }

    if (target_name == "default") {
        for (auto *target : info.targets) {
            if (target->isDefaultTarget) {
                return target;
            }
        }
    } else if (target_name == "release") {
        for (auto *target : info.targets) {
            if (target->targetName.ends_with("_release")) {
                return target;
            }
        }
    } else if (target_name == "debug") {
        for (auto *target : info.targets) {
            if (target->isDebugMode) {
                return target;
            }
        }
    }

    project_error(info);
    if (info.config.disableColors) {
        println("Could not find target {}.", target_name);
    } else {
        println("Could not find target {}{}{}.", HIGHLIGHT_COLOR, target_name, RESET_COLOR);
    }

    return nullptr;
}

void build(const std::string &target_name, ProjectInfo &info) {
    const TargetInfo *target = get_target_info(target_name, info);
    if (target == nullptr) {
        return;
    }

    if (info.config.verbosity >= Verbosity::Debug) {
        if (info.config.disableColors) {
            println("Building target {}.", target->targetName);
        } else {
            println("Building target {}{}{}.", HIGHLIGHT_COLOR, target->targetName, RESET_COLOR);
        }
    }

    compile(target, info);
}

void run(const std::string &target_name, ProjectInfo &info) {
    const TargetInfo *target = get_target_info(target_name, info);
    if (target == nullptr) {
        return;
    }

    build(target_name, info);
    if (info.buildFailed) {
        return;
    }

    if (info.config.verbosity >= Verbosity::Debug) {
        if (info.config.disableColors) {
            println("Running target {}.", target->targetName);
        } else {
            println("Running target {}{}{}.", HIGHLIGHT_COLOR, target->targetName, RESET_COLOR);
        }
    }
}

static bool create_folder(const std::string &folder) {
    return std::filesystem::create_directory(folder);
}

static bool create_build_file(const CreateProjectInfo &info) {
    const std::string build_file_name = info.name + "/" + NIFTY_BUILD_FILE;
    std::ofstream build_file(build_file_name);
    if (!build_file.is_open()) {
        println("Could not open {} for writing. Exiting.", build_file_name);
        return false;
    }

    build_file << "project = \"" << info.name << "\"" << std::endl;
    if (!info.author.empty()) {
        build_file << "author = \"" << info.author << "\"" << std::endl;
    }
    build_file << std::endl;

    build_file << "[" << info.name << "]" << std::endl;
    build_file << "description = \"" << info.name << " debug build.\"" << std::endl;
    build_file << "output_name = \"" << info.name << "\"" << std::endl;
    build_file << "entry_point = \"src/" << info.entryPoint << "\"" << std::endl;
    build_file << "debug = true" << std::endl;
    build_file << "default = true" << std::endl;

    build_file << std::endl;

    build_file << "[" << info.name << "_release]" << std::endl;
    build_file << "description = \"" << info.name << " release build.\"" << std::endl;
    build_file << "output_name = \"" << info.name << "\"" << std::endl;
    build_file << "entry_point = \"src/" << info.entryPoint << "\"" << std::endl;
    build_file << "optimization = \"fast\"" << std::endl;

    build_file.close();
    return true;
}

static bool create_entry_point(const CreateProjectInfo &info) {
    const std::string entry_point_name = info.name + "/src/" + info.entryPoint;
    std::ofstream entry_point(entry_point_name);
    if (!entry_point.is_open()) {
        println("Could not open {} for writing. Exiting.", entry_point_name);
        return false;
    }

    entry_point << "/-" << std::endl;
    entry_point << " - " << info.name << std::endl;
    if (!info.author.empty()) {
        const time_t t = time(nullptr);
        const struct tm tm = *localtime(&t);
        const int year = tm.tm_year + 1900;
        entry_point << " - Copyright (c) " << year << " " << info.author << std::endl;
    }
    entry_point << " -/" << std::endl;
    entry_point << std::endl;

    entry_point << "namespace main;" << std::endl;
    entry_point << std::endl;
    entry_point << "using fmt;" << std::endl;
    entry_point << std::endl;
    entry_point << "fn main() {" << std::endl;
    entry_point << "    println(\"Hullo!\");" << std::endl;
    entry_point << "}" << std::endl;

    entry_point.close();
    return true;
}

static bool create_readme(const CreateProjectInfo &info) {
    const std::string readme_name = info.name + "/README.md";
    std::ofstream readme(readme_name);
    if (!readme.is_open()) {
        println("Could not open {} for writing. Exiting.", readme_name);
        return false;
    }

    readme << "# " << info.name << std::endl;
    readme << "TODO: Useful information here." << std::endl;

    readme.close();
    return true;
}

static bool create_license(const CreateProjectInfo &info) {
    const std::string license_name = info.name + "/license.md";
    std::ofstream license(license_name);
    if (!license.is_open()) {
        println("Could not open {} for writing. Exiting.", license_name);
        return false;
    }

    license << "# TODO" << std::endl;

    license.close();
    return true;
}

static bool create_repo(const CreateProjectInfo &info) {
    if (info.noGit) {
        return true;
    }

    git_libgit2_init();

    git_repository *repo = nullptr;
    char *repo_name = str_new_fmt(nullptr, "%s", info.name.c_str());
    git_repository_init_options opts = GIT_REPOSITORY_INIT_OPTIONS_INIT;
    opts.description = str_new_fmt(nullptr, "%s - Nifty Project", info.name.c_str());
    opts.initial_head = "main";

    const int error = git_repository_init_ext(&repo, repo_name, &opts);
    str_delete(repo_name);

    if (error != 0) {
        println("Could not create repo for project {}. Exiting.", info.name);
        git_libgit2_shutdown();
        return false;
    }

    git_libgit2_shutdown();

    const std::string gitignore_name = info.name + "/.gitignore";
    std::ofstream gitignore(gitignore_name);
    if (!gitignore.is_open()) {
        println("Could not open {} for writing. Exiting.", gitignore_name);
        return false;
    }

    gitignore << "# Build" << std::endl;
    gitignore << "build-debug" << std::endl;
    gitignore << "build-release" << std::endl;
    gitignore << std::endl;
    gitignore << "# OS" << std::endl;
    gitignore << ".DS_Store" << std::endl;

    gitignore.close();
    return true;
}

static void create_project(const CreateProjectInfo &info) {
    if (!create_folder(info.name)) {
        println("Failed to create folder for project {}. Exiting.", info.name);
        return;
    }

    const std::string build_folder_name = info.name + "/" + "src";
    if (!create_folder(build_folder_name)) {
        println("Failed to create source folder for project {}. Exiting.", info.name);
        return;
    }

    const std::string debug_folder_name = info.name + "/" + "build-debug";
    if (!create_folder(debug_folder_name)) {
        println("Failed to create source folder for project {}. Exiting.", info.name);
        return;
    }

    if (!create_build_file(info)) {
        return;
    }

    if (!create_entry_point(info)) {
        return;
    }

    if (!create_readme(info)) {
        return;
    }

    if (!create_license(info)) {
        return;
    }

    if (!create_repo(info)) {
        return;
    }

    println("Created project {}.", info.name);
}

void new_project(const bool exists, const bool noGit) {
    if (exists) {
        println("A Nifty project already exists in this directory. Exiting.");
        return;
    }

    CreateProjectInfo info{};
    info.noGit = noGit;

    print("Project name: (Untitled) > ");
    std::getline(std::cin, info.name);
    if (info.name.empty()) {
        info.name = "Untitled";
    }

    print("Project version: (0.1.0) > ");
    std::getline(std::cin, info.version);
    if (info.version.empty()) {
        info.version = "0.1.0";
    }

    print("Entry point: (main.nifty) > ");
    std::getline(std::cin, info.entryPoint);
    if (info.entryPoint.empty()) {
        info.entryPoint = "main.nifty";
    }

    print("Author: > ");
    std::getline(std::cin, info.author);

    print("License: (zlib) > ");
    std::getline(std::cin, info.license);
    if (info.license.empty()) {
        info.license = "zlib";
    }

    constexpr int width = 25;
    dbln();
    printStringsWithSpacer("Project name", '-', info.name, width);
    printStringsWithSpacer("Project version", '-', info.version, width);
    printStringsWithSpacer("Entry point", '-', info.entryPoint, width);
    if (!info.author.empty()) {
        printStringsWithSpacer("Author", '-', info.author, width);
    }
    printStringsWithSpacer("License", '-', info.license, width);

    dbln();
    print("Is this ok? (yes) > ");
    std::string answer;
    std::getline(std::cin, answer);
    if (answer.empty()) {
        answer = "yes";
    }

    if (answer == "yes" || answer == "y") {
        create_project(info);
    } else {
        println("Did not create project {}. Exiting.", info.name);
    }
}

void list_targets(const ProjectInfo &info) {
    if (!info.loaded) {
        println("No project found, no targets to list.");
        return;
    }

    println("Targets in project {}:", info.name);

    int longestTargetName = 0;
    for (const auto *target : info.targets) {
        if (target->targetName.length() > longestTargetName) {
            longestTargetName = static_cast<int>(target->targetName.length());
        }
    }

    for (const auto *target : info.targets) {
        if (target->isDefaultTarget) {
            if (!info.config.disableColors) {
                printf("%s*", LINE_COLOR);
            }
        } else {
            fmt::print(" ");
        }

        const int width = std::max(longestTargetName + 5, 25);
        printStringsWithSpacer(target->targetName, '-', target->description, width);
        if (target->isDefaultTarget) {
            printf("%s", RESET_COLOR);
        }
    }
}
