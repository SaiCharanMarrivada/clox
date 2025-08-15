#ifndef VM_H
#define VM_H

#include "table.h"
#include "chunk.h"
#include "value.h"
#define STACK_MAX 256

typedef struct {
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *top;
    Table strings;
    Object *objects;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;


extern VM vm;

void init_vm();
void free_vm();
InterpretResult interpret(const char *source);

void push(Value value);
Value pop();

#endif
