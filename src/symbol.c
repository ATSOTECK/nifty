//
// Created by Skyler Burwell on 5/22/25.
//

#include "symbol.h"

SymbolTable *newSymbolTable() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->table = tableCreate();
    table->parent = nullptr;

    return table;
}

Symbol *getSymbol(const SymbolTable *table, const char *name) {
    Symbol *symbol = tableGet(table->table, name);
    if (symbol == nullptr && table->parent != nullptr) {
        symbol = getSymbol(table->parent, name);
    }

    return symbol;
}

bool symbolExists(const SymbolTable *table, const char *name) {
    bool exists = tableExists(table->table, name);
    if (!exists && table->parent != nullptr) {
        exists = symbolExists(table->parent, name);
    }

    return exists;
}
