#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define LOAD_FACTOR 0.75

void init_table(Table *table) {
    table->count = 0;
    table->capacity = 0;
    table->keys = NULL;
    table->values = NULL;
}

void free_table(Table *table) {
    FREE_ARRAY(String *, table->keys, table->capacity);
    FREE_ARRAY(Value, table->values, table->capacity);
    init_table(table);
}

void grow_and_copy(Table *table, int new_capacity) {
    String **keys = ALLOCATE(String *, new_capacity);
    Value *values = ALLOCATE(Value, new_capacity);

    for (int i = 0; i < new_capacity; i++) {
        keys[i] = NULL;
        values[i] = NIL_VAL;
    }
    String **old_keys = table->keys;
    Value *old_values = table->values;

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        if (old_keys[i] == NULL) {
            continue;
        }
        int index = old_keys[i]->hash & (new_capacity - 1); 
        table->count++;
        for (;;) {
            if (keys[index] == NULL) {
                keys[index] = old_keys[i];
                values[index] = old_values[i];
                break;
            }
            index = (index + 1) & (new_capacity - 1);
        }
    }
    FREE_ARRAY(String *, old_keys, table->capacity);
    FREE_ARRAY(Value, old_values, table->capacity);
    table->capacity = new_capacity;
    table->keys = keys;
    table->values = values;
}


bool table_set(Table *table, String *key, Value value) {
    if (table->count + 1 > table->capacity * LOAD_FACTOR) {
        int capacity = GROW_CAPACITY(table->capacity);
        grow_and_copy(table, capacity);
    }
    int capacity = table->capacity;
    uint32_t index = key->hash & (capacity - 1);
    String **keys = table->keys;
    for (;;) {
        if (keys[index] == NULL) {
            // count stays same if it is a tombstone
            if (IS_NIL(table->values[index])) {
                table->count++;
            }
            keys[index] = key;
            table->values[index] = value;
            return true;
        } else if (keys[index] == key) {
            return false;
        }
        index = (index + 1) & (capacity - 1);
    }
    UNREACHABLE();
}

bool table_get(Table *table, String *key, Value *value) {
    if (table->count == 0) {
        return false;
    }
    int capacity = table->capacity;
    String **keys = table->keys;
    int index = key->hash & (capacity - 1);
    for (;;) {
        if (keys[index] == NULL) {
            if (IS_NIL(table->values[index])) {
                return false;
            // continue probing if it is a tombstone
            } else {
                continue;
            }
          // found key
        } else if (keys[index] == key) {
            *value = table->values[index];
            return true;
        }
        index = (index + 1) & (capacity - 1);
    }
    UNREACHABLE();
}

bool table_delete(Table *table, String *key) {
    if (table->count == 0) {
        return false;
    }
    int capacity = table->capacity;
    int index = key->hash & (capacity - 1);
    String **keys = table->keys;
    for (;;) {
        if (keys[index] == key) {
            // replace with a tombstone
            keys[index] = NULL;
            table->values[index] = BOOL_VAL(true);
            return true;
        } else if (keys[index] == NULL) {
            return false;
        }
        index = (index + 1) & (capacity - 1);
    }
    UNREACHABLE();
}

String* table_find_string(
    Table *table, const char *data, int length, uint32_t hash
) {
    if (table->count == 0) {
        return NULL;
    }
    int capacity = table->capacity;
    uint32_t index = hash & (capacity - 1);
    String **keys = table->keys;
    for (;;) {
        if (keys[index] == NULL) {
            if (IS_NIL(table->values[index])) {
                return NULL;
            }
        } else if (keys[index]->hash == hash
              && keys[index]->length == length
              && memcmp(keys[index]->data, data, length) == 0
        ) {
            return keys[index];
        }
        index = (index + 1) & (capacity - 1);
    }
}
