#ifndef STACK_INCLUDED
#define STACK_INCLUDED
#include <stdint.h>

struct Stack {
        uint32_t *data;
        unsigned size;
        unsigned used;
        unsigned cur;
};
typedef struct Stack *Stack;
Stack stack_init();
void stack_push(Stack S, unsigned u);
uint32_t stack_pop(Stack S);
void stack_free(Stack S);
int stack_empty(Stack S);
#endif
