#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "value.h"

#define IS_STRING(value) (is_objecttype(value, STRING))
#define AS_CSTRING(value) ((AS_STRING(value))->data)
#define AS_STRING(value) (((String *)AS_OBJECT(value)))

typedef enum {
    STRING
} ObjectType;

struct Object {
    ObjectType type;
    struct Object *next;
};

struct String {
    Object object;
    int length;
    char *data;
};

static inline bool is_objecttype(Value value, ObjectType type) {
    return IS_OBJECT(value) && AS_OBJECT(value)->type == type;
}

String *copy_string(const char *data, int length);
String *take_string(char *data, int length);
void print_object(Value value);
#endif

