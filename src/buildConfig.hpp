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

#ifndef NIFTY_BUILDCONFIG_HPP
#define NIFTY_BUILDCONFIG_HPP

#include "util/str.hpp"

#include <vector>

struct BuildConfig {
    String targetName;
    String outputName;
    String entryPoint;
    bool noEntryPoint;
    bool noDepreciated;
    bool noGoto;
    bool debug;
    std::vector<String> defines;
    bool errorOnWarn;
    bool noWarn;
    String optimization;
    bool isDefault;
    bool noBoundsCheck;
    std::vector<String> children;
    bool disallowNull;
    int macroRecursionDepth;
    std::vector<String> executeBeforePlatforms;
    std::vector<String> executeBeforeCmds;
    std::vector<String> executeAfterPlatforms;
    std::vector<String> executeAfterCmds;
    int deprecatedAfterWarnWithin;
};

BuildConfig getDefaultBuildConfig();

#endif //NIFTY_BUILDCONFIG_HPP
