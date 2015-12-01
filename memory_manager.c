/*
 * memory_manager.c
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Initializes and frees memory structure
 */
#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include "bitpack.h"


/* 
 * Creates a memory struct that holds information about the UM's memory and
 * initializes all data structures within the larger data structure.
 * Mem struct gets passed around as a pointer between various functions
 * throughout the entire UM implimentation.
 */
Mem initialize_memory(FILE *input, int len)
{
        Mem memory = malloc(sizeof(struct Mem));
        for (int i = 0; i < NUMREGS; i++)
                memory->regs[i] = 0;
        memory->main_mem = Seq_new(10);
        Seg seg0 = UArray_new(len, REGSIZE);
        memory->free_regs = Stack_new();
        memory->pcount = 0;
         
        unsigned c; 
        unsigned inst;
        unsigned lsb = 0;
        for (int i = 0; i < len; i++) {
                inst = 0;
                for (int j = 0; j < 4; j++) {
                        c = getc(input);
                        lsb = 24-j*8;
                        inst = (unsigned)Bitpack_newu(inst, 8, lsb, c);
                }
                *((unsigned*)UArray_at(seg0, i)) = inst;
        }
        fclose(input);
        Seq_addhi(memory->main_mem, seg0);
        return memory;
}

/* frees all data structures within the Mem struct. */
void free_memory(Mem memory)
{
        Seg segment;
        segment = (Seg)Seq_remlo(memory->main_mem);
        while (Seq_length(memory->main_mem) > 0) {
                segment = (Seg)Seq_remlo(memory->main_mem);
                if (segment != NULL)
                        UArray_free(&segment);
        }
        Stack_free(&memory->free_regs);
        Seq_free(&memory->main_mem);
        free(memory);
}
