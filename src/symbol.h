//
// Created by Skyler Burwell on 5/18/25.
//

#ifndef SYMBOL_H
#define SYMBOL_H

#include "common.h"

typedef struct {
    //
} NamespaceInfo;

typedef struct {
    //
} PackageInfo;

typedef enum {
    //
} SymbolKind;

typedef enum {
    ScopeFile,      // ends in underscore
    ScopeNamespace, // starts with underscore
    ScopeExported   // doesn't start or end with underscore
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

typedef struct {
    int count;
    int capacity;
    Symbol *symbols;
    struct SymbolTable *parent;
} SymbolTable;

Symbol *getSymbol(const char *name, const char *namespaceName, const char *packageName);

#endif //SYMBOL_H
