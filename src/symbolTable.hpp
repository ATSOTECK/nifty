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

#ifndef __NIFTY_SYMBOLTABLE_HPP__
#define __NIFTY_SYMBOLTABLE_HPP__

#include "util/str.hpp"

#include <vector>
#include <unordered_map>

enum SymbolPrivilegeLevel {
	Default, // Non package related.
    Export,
    Private,
    Local
};

// So we can have two symbols with the same name as long as their kind differs.
// May get rid of this as it could be confusing.
enum SymbolKind {
    Function,
    Variable,
    FunctionPointer, // Special kind of variable.
    Enum,
    Struct
};

struct Symbol {
    String name;
    String type;
    SymbolKind kind;
	bool isNull;
    bool isConst;
    int line;
    int linePos;
	String file;
    SymbolPrivilegeLevel privilegeLevel;
};

class SymbolTable {
public:
    SymbolTable();
    SymbolTable(SymbolTable *container);

    [[nodiscard]] bool contains(const String &name, SymbolKind kind) const;
    bool add(const Symbol &symbol);


private:
    SymbolTable *_containing;
    std::vector<SymbolTable *> _tables;
    std::unordered_map<String, Symbol> _symbols;
};




#endif //__NIFTY_SYMBOLTABLE_HPP__
