/*
 * memory_manager.h
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Initializes and frees memory structure
 */
#ifndef MEMORY_MANAGER_INCLUDED 
#define MEMORY_MANAGER_INCLUDED
#include <stdio.h>
#include "uarray.h"
#include "seq.h"
#include "stack.h"

typedef UArray_T Seg;

typedef struct Mem {
        UArray_T registers;
        Seq_T main_mem;
        Stack_T free_regs;
        unsigned pcount;
} *Mem;

Mem initialize_memory(FILE *input, int len);
void free_memory(Mem memory);
unsigned * get_register(Mem memory, int reg_num);

#endif
