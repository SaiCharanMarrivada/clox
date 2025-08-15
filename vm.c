#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include "vm.h"
#include "object.h"
#include <string.h>
#include <time.h>
#include "memory.h"

VM vm;

#ifdef DEBUG_TRACE_EXECUTION
#define INSPECT_STACK()   \
    do { \
        printf("          "); \
        for (Value *slot = vm.stack; slot < vm.top; slot++) { \
            print_value(*slot); \
            printf(", "); \
        } \
        printf("\n"); \
        disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code)); \
    } while (false)
#else
#define INSPECT_STACK()
#endif


void init_vm() {
    vm.top = vm.stack;
    vm.objects = NULL;
    init_table(&vm.strings);
}

void free_vm() {
    free_table(&vm.strings);
    free_objects();
}

static void runtime_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t index = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[index];
    fprintf(stderr, "[line %d] in script\n", line);
    vm.top = vm.stack;
}

void push(Value value) {
    *vm.top = value;
    vm.top++;
}

Value pop() {
    vm.top--;
    return *vm.top;
}

bool is_false(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

static InterpretResult run() {
#define DISPATCH() \
    do { \
        INSPECT_STACK(); \
        goto *dispatch_table[*vm.ip++]; \
    } while (false)

#define BINARY_OP(value_type, op) \
    do { \
        INSPECT_STACK(); \
        Value b = pop(); \
        Value a = pop(); \
        if (!IS_NUMBER(b) || !IS_NUMBER(a)) { \
            runtime_error("Operands must be numbers"); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        push(value_type(AS_NUMBER(a) op AS_NUMBER(b))); \
        DISPATCH(); \
    } while (false)

    static void **dispatch_table[] = {
       [OP_CONSTANT] = &&CONSTANT,
       [OP_RETURN] = &&RETURN,
       [OP_NEGATE] = &&NEGATE,
       [OP_ADD] = &&ADD,
       [OP_SUBTRACT] = &&SUBTRACT,
       [OP_MULTIPLY] = &&MULTIPLY,
       [OP_DIVIDE] = &&DIVIDE,
       [OP_TRUE] = &&TRUE,
       [OP_FALSE] = &&FALSE,
       [OP_NIL] = &&NIL,
       [OP_NOT] = &&NOT,
       [OP_EQUAL] = &&EQUAL,
       [OP_GREATER] = &&GREATER,
       [OP_LESS] = &&LESS
    };
    DISPATCH();

CONSTANT:
    Value constant = vm.chunk->constants.values[*vm.ip++];
    push(constant);
    DISPATCH();

NEGATE:
    if (IS_NUMBER(vm.top[-1])) {
        push(NUMBER_VAL(-AS_NUMBER(pop())));
        DISPATCH();
    } else {
        runtime_error("Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
    }

TRUE:
    push(BOOL_VAL(true));
    DISPATCH();

FALSE:
    push(BOOL_VAL(false));
    DISPATCH();

NIL:
    push(NIL_VAL);
    DISPATCH();

NOT:
    push(BOOL_VAL(is_false(pop())));
    DISPATCH();

EQUAL:
    Value b = pop();
    Value a = pop();
    push(BOOL_VAL(is_equal(a, b)));
    DISPATCH();

GREATER:
    BINARY_OP(BOOL_VAL, >);

LESS:
    BINARY_OP(BOOL_VAL, <);

ADD: {
    Value b = pop();
    Value a = pop();
    if (IS_STRING(a) && IS_STRING(b)) {
        String *s1 = AS_STRING(a);
        String *s2 = AS_STRING(b);
        int length = s1->length + s2->length;
        char *data = ALLOCATE(char, length + 1);
        memcpy(data, s1->data, s1->length);
        memcpy(data + s1->length, s2->data, s2->length);
        data[length] = '\0';
        String *result = take_string(data, length);
        push(OBJECT_VAL(result));
        DISPATCH();
    } else if (IS_NUMBER(a) && IS_NUMBER(b)) {
        push(NUMBER_VAL(AS_NUMBER(a) + AS_NUMBER(b)));
        DISPATCH();
    } else {
        runtime_error("Only strings or numbers are allowed.");
        return INTERPRET_RUNTIME_ERROR;
    }
}

SUBTRACT:
    BINARY_OP(NUMBER_VAL, -);

MULTIPLY:
    BINARY_OP(NUMBER_VAL, *);

DIVIDE:
    BINARY_OP(NUMBER_VAL, /);

RETURN:
    print_value(pop());
    return INTERPRET_OK;
}
#pragma GCC diagnostic pop

InterpretResult interpret(const char *source) {
    Chunk chunk;
    init_chunk(&chunk);
    // try to compile the source
    if (!compile(source, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();
    free_chunk(&chunk);
    return result;
}


