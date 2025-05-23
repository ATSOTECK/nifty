//
// Created by Skyler Burwell on 5/22/25.
//

#include "table.h"

#include "util/str.h"

static uint32_t hashKey(const char *key) {
    uint32_t hash = 5381;
    uint32_t c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

static const char *setEntry(Entry *entries, const size_t capacity, const char *key, void *value, size_t *pLen) {
    const uint32_t hash = hashKey(key);
    size_t index = hash & (uint64_t)(capacity - 1);

    while (entries[index].key != NULL) {
        if (str_eq(key, entries[index].key)) {
            entries[index].value = value;
            return entries[index].key;
        }

        index++;
        if (index >= capacity) {
            index = 0;
        }
    }

    if (pLen != nullptr) {
        key = str_dup(key);
        if (key == nullptr) {
            return nullptr;
        }
        (*pLen)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;

    return key;
}

static bool expand(Table *table) {
    const size_t newCapacity = table->capacity * 2;
    if (newCapacity < table->capacity) {
        return false;
    }

    Entry * newEntries = calloc(newCapacity, sizeof(Entry));
    if (newEntries == NULL) {
        return false;
    }

    for (size_t i = 0; i < table->capacity; ++i) {
        const Entry entry = table->entries[i];
        if (entry.key != NULL) {
            setEntry(newEntries, newCapacity, entry.key, entry.value, NULL);
        }
    }

    free(table->entries);
    table->entries = newEntries;
    table->capacity = newCapacity;

    return true;
}

Table *tableCreate() {
    Table *table = malloc(sizeof(Table));
    if (table == nullptr) {
        return nullptr;
    }

    table->len = 0;
    table->capacity = 64;
    table->entries = calloc(table->capacity, sizeof(Entry));
    if (table->entries == nullptr) {
        free(table);
        return nullptr;
    }

    return table;
}

void tableFree(Table *table) {
    for (size_t i = 0; i < table->capacity; ++i) {
        free((void*)table->entries[i].key);
    }

    free(table->entries);
    free(table);
}

void *tableGet(const Table *table, const char *key) {
    const uint32_t hash = hashKey(key);
    size_t index = hash & (uint64_t)(table->capacity - 1);
    while (table->entries[index].key != nullptr) {
        if (str_eq(key, table->entries[index].key)) {
            return table->entries[index].value;
        }

        index++;
        if (index >= table->capacity) {
            index = 0;
        }
    }

    return nullptr;
}

bool tableExists(const Table *table, const char *key) {
    const uint32_t hash = hashKey(key);
    size_t index = hash & (uint64_t)(table->capacity - 1);
    while (table->entries[index].key != nullptr) {
        if (str_eq(key, table->entries[index].key)) {
            return true;
        }

        index++;
        if (index >= table->capacity) {
            index = 0;
        }
    }

    return false;
}

const char *tableSet(Table *table, const char *key, void *value) {
    if (table == nullptr || table->entries == nullptr || value == nullptr) {
        return nullptr;
    }

    if (table->len >= table->capacity / 2) {
        if (!expand(table)) {
            return nullptr;
        }
    }

    return setEntry(table->entries, table->capacity, key, value, &table->len);
}
