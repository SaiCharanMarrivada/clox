#include <stdbool.h>
#include <stdio.h>
#include "memory.h"
#include "value.h"
#include "object.h"
#include "string.h"

inline void init_value_array(ValueArray *array, bool with_capacity) {
    array->count = 0;
    array->capacity = with_capacity ? 8 : 0;
    array->values = with_capacity ? ALLOCATE(Value, 8) : NULL;
}

void write_value_array(ValueArray *array, Value value) {
    if UNLIKELY(array->capacity < array->count + 1) {
        int old_capacity = array->capacity;
        array->capacity = 2 * old_capacity;
        array->values =
            GROW_ARRAY(Value, array->values, old_capacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void free_value_array(ValueArray *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array, false);
}

bool is_equal(Value a, Value b) {
    // SAFETY: This is safe because every Value is constructed via
    // designated initialization. That guarantees that any fields
    // not explicitly set — including padding bytes — are zeroed.
    // Therefore the entire object representation is deterministic,
    // and `memcmp(&a, &b, sizeof(Value))` is a correct equality check.
    return memcmp(&a, &b, sizeof(Value)) == 0;
}

void print_value(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NIL:
            printf("nil");
            break;
        case VAL_NUMBER:
            printf("%g", AS_NUMBER(value));
            break;
        case VAL_OBJECT:
            print_object(value);
            break;
    }
}
