#ifndef TABLE_H
#define TABLE_H

#include "common.h"
#include "value.h"

typedef struct {
    int count;
    int capacity;
    String **keys;
    Value *values;
} Table;

void init_table(Table *table, bool with_capacity);
void free_table(Table *table);
bool table_set(Table *table, String *key, Value value);
bool table_get(Table *table, String *key, Value *value);
String *table_find_string(
    Table *table, const char *data, int length, uint32_t hash
);

#endif
