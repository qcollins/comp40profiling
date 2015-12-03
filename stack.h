#include <stdint.h>
#include <stdlib.h>
#include <mem.h>

struct Stack {
        uint32_t *data;
        unsigned size;
        unsigned used;
};
typedef struct Stack Stack;

void stack_init(Stack *S)
{
        S->size = 64;
        S->used = 0;
}

void stack_push(Stack *S, unsigned u)
{
        if (S->used < S->size)
                S->data[S->used++] = u;
        else {
                S->size *= 2;
                uint32_t new_data[S->size];
                for(unsigned i = 0; i < S->size/2; i++) {
                        new_data[i] = S->data[i];
                }
                uint32_t *temp = S->data;
                S->data = new_data;
                free(temp);
        }
}

uint32_t stack_pop(Stack *S)
{
        return S->data[S->used--];
}

void stack_free(Stack *S)
{
             
}
