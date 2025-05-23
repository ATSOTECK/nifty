//
// Created by Skyler Burwell on 5/18/25.
//

#ifndef SYMBOL_H
#define SYMBOL_H

#include "common.h"

#include "table.h"

typedef struct {
    //
} NamespaceInfo;

typedef struct {
    //
} PackageInfo;

typedef enum {
    SymbolInt,

    SymbolType, // The symbol is a type
} SymbolKind;

typedef enum {
    ScopeFile,      // Ends in underscore
    ScopeNamespace, // Starts with underscore
    ScopeExported   // Doesn't start or end with underscore
} Scope;

typedef struct {
    int value;
} IntSymbol;

typedef struct {
    char *name;
    char *fileName;
    SymbolKind kind;

    bool mutable;
    bool valueSet;

    union {
        int intValue;
    } data;

} Symbol;

typedef struct SymbolTable SymbolTable;

typedef struct SymbolTable {
    Table *table;
    SymbolTable *parent;
} SymbolTable;

SymbolTable *newSymbolTable();
Symbol *getSymbol(const SymbolTable *table, const char *name);
bool symbolExists(const SymbolTable *table, const char *name);

#endif //SYMBOL_H
