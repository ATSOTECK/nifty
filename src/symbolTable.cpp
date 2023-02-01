/*
 * Nifty - Nifty Programming Language
 * Copyright (c) 2022 Skyler Burwell
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

#include "symbolTable.hpp"

#define MAX_DEPTH 256

SymbolTable::SymbolTable():
    _containing(nullptr)
{
    //
}

SymbolTable::SymbolTable(SymbolTable *container):
    _containing(container)
{
    //
}

bool SymbolTable::contains(const String &name, SymbolKind kind) const {
    const auto idx = _symbols.find(name);
    if (idx != _symbols.end()) {
        return true;
    }

    SymbolTable *container = _containing;
    int count = 0; // TODO: Should this error if at MAX_DEPTH?
    while (container != nullptr && count++ < MAX_DEPTH) {
        if (container->contains(name, kind)) {
            return true;
        }

        container = container->_containing;
    }

    return false;
}

bool SymbolTable::add(const Symbol &symbol) {
    if (!contains(symbol.name, symbol.kind)) {
        _symbols[symbol.name] = symbol;
        return true;
    }

    return false;
}
