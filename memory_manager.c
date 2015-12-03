/*
 * memory_manager.c
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Initializes and frees memory structure
 */
#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "bitpack_inline.h"


/* 
 * Creates a memory struct that holds information about the UM's memory and
 * initializes all data structures within the larger data structure.
 * Mem struct gets passed around as a pointer between various functions
 * throughout the entire UM implimentation.  */
Mem initialize_memory(FILE *input, int len)
{
        Mem memory = (Mem)calloc(1, sizeof(struct Mem));
        /*
        for (int i = 0; i < NUMREGS; i++)
                memory->regs[i] = 0;
                */
        //memory->main_mem = Seq_new(10);
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

/* frees all data structures within the Mem struct. */
void free_memory(Mem memory)
{
        Seg *main_mem = memory->main_mem;
        Seg segment = main_mem[0];
        if (memory->news0 == 0)
                free(segment);
        for (unsigned i = 1; i < memory->hi_seg; i++) {
                if (main_mem[i] != 0)
                        free(main_mem[i]);
        }
        /*
        */
        stack_free(memory->reuse_segs);
        free(main_mem);
        free(memory);
}
