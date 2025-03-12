#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

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
}

void free_vm() {

}

void push(Value value) {
    *vm.top = value;
    vm.top++;
}

Value pop() {
    vm.top--;
    return *vm.top;
}


static InterpretResult run() {
#define DISPATCH() \
    do { \
        INSPECT_STACK(); \
        goto *dispatch_table[*vm.ip++]; \
    } while (false)

#define BINARY_OP(op) \
    do { \
        INSPECT_STACK(); \
        Value b = pop(); \
        Value a = pop(); \
        push(a op b); \
        DISPATCH(); \
    } while (false)

    static void **dispatch_table[] = {
       [OP_CONSTANT] = &&CONSTANT,
       [OP_RETURN] = &&RETURN,
       [OP_NEGATE] = &&NEGATE,
       [OP_ADD] = &&ADD,
       [OP_SUBTRACT] = &&SUBTRACT,
       [OP_MULTIPLY] = &&MULTIPLY,
       [OP_DIVIDE] = &&DIVIDE
    };
    DISPATCH();

CONSTANT:
    Value constant = vm.chunk->constants.values[*vm.ip++];
    push(constant);
    DISPATCH();

NEGATE:
    push(-pop());
    DISPATCH();

ADD:
    BINARY_OP(+);

SUBTRACT:
    BINARY_OP(-);

MULTIPLY:
    BINARY_OP(*);

DIVIDE:
    BINARY_OP(/);

RETURN:
    print_value(pop());
    return INTERPRET_OK;
}

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


