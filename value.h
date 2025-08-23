#ifndef VALUE_H
#define VALUE_H

#include "common.h"
typedef struct Object Object;
typedef struct String String;

typedef enum {
    VAL_NIL,
    VAL_BOOL,
    VAL_NUMBER,
    VAL_OBJECT
} ValueType;


typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Object *object;
    } as;
} Value;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJECT(value) ((value).type == VAL_OBJECT)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJECT(value) ((value).as.object)

#define BOOL_VAL(value) \
    ((Value){.type = VAL_BOOL, .as = {.boolean = value}})
#define NIL_VAL ((Value){.type = VAL_NIL, .as = {.number = 0}})
#define NUMBER_VAL(value) \
    ((Value){.type = VAL_NUMBER, .as = {.number = value}})
#define OBJECT_VAL(value) \
    ((Value){.type = VAL_OBJECT, .as = {.object = (Object *)value}})
#define OBJECT_TYPE(value) (AS_OBJECT(value)->type)

typedef struct {
    int capacity;
    int count;
    Value *values;
} ValueArray;

void init_value_array(ValueArray *array, bool with_capacity);
void write_value_array(ValueArray *array, Value value);
void free_value_array(ValueArray *array);
bool is_equal(Value a, Value b);
void print_value(Value value);
#endif

