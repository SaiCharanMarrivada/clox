#include <stdlib.h>
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

void *reallocate(void *pointer, UNUSED size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL) exit(1);
    return result;
}

void free_object(Object *object) {
    switch (object->type) {
        case STRING: {
            String *string = (String *)object;
            reallocate(string, sizeof(String) + string->length + 1, 0);
            break;
        }
    }
}

void free_objects() {
    Object *object = vm.objects;
    while (object) {
        Object *next = object->next;
        free_object(object);
        object = next;
    }
}
