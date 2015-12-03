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
        //Seq_T main_mem;
        Seg *main_mem;
        Stack reuse_segs;
        unsigned mem_size;
        unsigned hi_seg;
        unsigned pcount;
        int news0;
} *Mem;


static inline Mem initialize_memory(FILE *input, int len)
{
        Mem memory = (Mem)calloc(1, sizeof(struct Mem));
        memory->mem_size = 32;
        // Seg *main_mem = (Seg *)calloc(memory->mem_size, 8);
        Seg *main_mem = (Seg *)malloc(memory->mem_size * 8);
        Seg seg0 = (Seg)malloc(len * REGSIZE);  
        //Seg seg0 = calloc(len, REGSIZE);
        // stack_init(memory->reuse_segs);
        memory->reuse_segs = stack_init();
        memory->pcount = 0;
        memory->news0 = 0;
        memory->hi_seg = 1;
         
        unsigned c; 
        unsigned inst;
        unsigned lsb = 0;
        for (int i = 0; i < len; i++) {
                inst = 0;
                for (int j = 0; j < 4; j++) {
                        c = getc(input);
                        lsb = 24-j*8;
                        inst = bitpack_newu(inst, 8, lsb, c);
                }
                seg0[i] = inst;
        }
        //unsigned length = sizeof(seg0)/REGSIZE;
        fclose(input);
        main_mem[0] = seg0;
        memory->main_mem = main_mem;
        return memory;
}

static inline void free_memory(Mem memory)
{
        Seg *main_mem = memory->main_mem;
        Seg segment = main_mem[0];
        if (memory->news0 == 0)
                free(segment);
        for (unsigned i = 1; i < memory->mem_size; i++) {
                if (main_mem[i] != 0)
                        free(main_mem[i]);
        }
        stack_free(memory->reuse_segs);
        free(main_mem);
        free(memory);
}

#endif
