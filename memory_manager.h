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
#include "bitpack_inline.h"

#define REGSIZE 4
#define NUMREGS 8

typedef uint32_t* Seg;

struct Mem {
        uint32_t regs[NUMREGS];
        Seg *main_mem;
        Stack reuse_segs;
        unsigned mem_size;
        unsigned hi_seg;
        unsigned pcount;
        int news0;
};

#endif
