#include <stdlib.h>
#include "memory.h"
#include "object.h"

void *reallocate(void *pointer, size_t old_size, size_t new_size) {
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
        case STRING:
            String *string = (String *)object;
            FREE_ARRAY(char, string->data, string->length + 1);
            FREE(String, object);
            break;
    }
}
