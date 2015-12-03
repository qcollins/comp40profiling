/*
 * memory_manager.h
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Initializes and frees memory structure
 */
#ifndef MEMORY_MANAGER_INCLUDED 
#define MEMORY_MANAGER_INCLUDED
#include <stdio.h>
#include "uint_stack.h"
#include <stdint.h>

#define REGSIZE 4
#define NUMREGS 8

typedef uint32_t* Seg;

typedef struct Mem {
        uint32_t regs[NUMREGS];
        //Seq_T main_mem;
        Seg *main_mem;
        Stack reuse_segs;
        unsigned mem_size;
        unsigned hi_seg;
        unsigned pcount;
        int news0;
} *Mem;

Mem initialize_memory(FILE *input, int len);
void free_memory(Mem memory);

#endif
