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

typedef struct Mem {
        uint32_t regs[NUMREGS];
        Seg *main_mem;
        Stack reuse_segs;
        unsigned mem_size;
        unsigned hi_seg;
        unsigned pcount;
        int news0;
} Mem;

Mem memory;

static inline void free_memory()
{
        Seg *main_mem = memory.main_mem;
        Seg segment = main_mem[0];
        if (memory.news0 == 0)
                free(segment);
        for (unsigned i = 1; i < memory.hi_seg; i++) {
                if (main_mem[i] != NULL)
                        free(main_mem[i]);
        }
        stack_free(memory.reuse_segs);
        free(main_mem);
}

typedef struct Inst {
        uint32_t opcode;
        uint32_t ra;
        uint32_t rb;
        uint32_t rc;
} Inst;
Inst *cmds;

#endif
