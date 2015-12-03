#ifndef STACK_INCLUDED
#define STACK_INCLUDED
#include <stdint.h>
#include <stdlib.h>

struct Stack {
        uint32_t *data;
        unsigned size;
        unsigned used;
        unsigned cur;
};
typedef struct Stack *Stack;
/*
static inline Stack stack_init();
static inline void stack_push(Stack S, unsigned u);
static inline uint32_t stack_pop(Stack S);
static inline void stack_free(Stack S);
static inline int stack_empty(Stack S);
*/
static inline Stack stack_init()
{
        Stack S;
        S = malloc(sizeof(struct Stack));
        S->size = 64;
        S->data = malloc(4*S->size);
        S->used = 0;
        S->cur = 0;
        return S;
}

static inline void stack_push(Stack S, unsigned u)
{
        if (S->used >= S->size) {
                uint32_t *new_data = malloc(8*S->size);
                for(unsigned i = 0; i < S->size; i++) {
                        new_data[i] = S->data[(S->cur+i)%S->size];
                }
                S->cur = 0;
                S->size *= 2;
                uint32_t *temp = S->data;
                S->data = new_data;
                free(temp);
        }
        S->data[S->cur] = u;
        S->cur = (S->cur+1)%(S->size);
}

static inline uint32_t stack_pop(Stack S)
{
        S->cur = (S->cur-1)%S->size;
        uint32_t popped = S->data[S->cur];
        S->used--;
        return popped;
}

static inline void stack_free(Stack S)
{
        free(S->data);
        free(S);
}

static inline int stack_empty(Stack S)
{
        if (S->used == 0)
                return 1;
        return 0;
}
#endif
