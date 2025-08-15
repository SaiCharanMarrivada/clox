#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJECT(object, type) \
    (object *)allocate_object(sizeof(object), type)

static uint32_t hash_string(const char *key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

static Object *allocate_object(size_t size, ObjectType type) {
    Object *object = (Object *)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

String *allocate_string(char *buffer, int length, uint32_t hash) {
    String *string = ALLOCATE_OBJECT(String, STRING);
    string->length = length;
    string->data = buffer;
    string->hash = hash;
    table_set(&vm.strings, string, NIL_VAL);
    return string;
}

String *take_string(char *buffer, int length) {
    uint32_t hash = hash_string(buffer, length);
    String *interned = table_find_string(&vm.strings, buffer, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, buffer, length + 1);
        return interned;
    }
    return allocate_string(buffer, length, hash);
}

String *copy_string(const char *data, int length) {
    uint32_t hash = hash_string(data, length);
    String *interned = table_find_string(&vm.strings, data, length, hash);
    if (interned != NULL) {
        return interned;
    }
    char *buffer = ALLOCATE(char, length + 1); // +1 for NULL character
    memcpy(buffer, data, length);
    buffer[length] = '\0';
    return allocate_string(buffer, length, hash);
}

void print_object(Value value) {
    switch (OBJECT_TYPE(value)) {
        case STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
