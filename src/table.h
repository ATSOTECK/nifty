//
// Created by Skyler Burwell on 5/22/25.
//

#ifndef TABLE_H
#define TABLE_H

#include "common.h"

#include <stdlib.h>

typedef struct {
    const char *key;
    void *value;
} Entry;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t len;
} Table;

Table *tableCreate(size_t capacity);
void tableFree(Table *table);
void *tableGet(const Table *table, const char *key);
bool tableExists(const Table *table, const char *key);
const char *tableSet(Table *table, const char *key, void *value);

#endif //TABLE_H
