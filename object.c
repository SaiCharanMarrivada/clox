#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJECT(object, type) \
    (object *)allocate_object(sizeof(object), type)

static Object *allocate_object(size_t size, ObjectType type) {
    Object *object = (Object *)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

String *allocate_string(char *buffer, int length) {
    String *string = ALLOCATE_OBJECT(String, STRING);
    string->length = length;
    string->data = buffer;
}


String *copy_string(const char *data, int length) {
    char *buffer = ALLOCATE(char, length + 1); // +1 for NULL character
    memcpy(buffer, data, length);
    buffer[length] = '\0';
    return allocate_string(buffer, length);
}

void print_object(Value value) {
    switch (OBJECT_TYPE(value)) {
        case STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
