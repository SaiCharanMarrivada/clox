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

String *make_string(int length) {
    String *string = 
        (String *)allocate_object(sizeof(String) + length + 1, STRING);
    string->length = length;
    return string;
}

String *copy_string(const char *buffer, int length) {
    String *string = make_string(length);
    string->hash = hash_string(buffer, length);
    memcpy(string->data, buffer, length);
    string->data[length] = '\0';
    table_set(&vm.strings, string, NIL_VAL);
    return string;
}

void print_object(Value value) {
    switch (OBJECT_TYPE(value)) {
        case STRING:
            printf("%s", AS_CSTRING(value));
            break;
        default:
            UNREACHABLE();
    }
}
