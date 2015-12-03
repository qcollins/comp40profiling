/*
 * um.c
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Main method for a universal machine: a computer emulated in memory that
 * contains 8 32-bit registers and can create segments of memory as needed.
 */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include "memory_manager.h"
#include <stdint.h>
#include "um_methods.h"
#include "bitpack_inline.h"
#include <time.h>

static inline void initialize_memory(FILE *input, int len)
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

static inline void free_memory()
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


int main (int argc, char **argv)
{ FILE *input;
        if (argc != 2) {
                printf("You must provide a single filename as an argument!\n");
                return 1;
        }
        if ((input = fopen(argv[1], "r")) == NULL) {
                printf("Invalid filename\n");
                return 1;
        }

        int fsize;
        struct stat buffer;
        stat(argv[1], &buffer);
        fsize = (int)buffer.st_size;
        
        Mem memory = initialize_memory(input, fsize/4);
        uint32_t cmd = 0;
        /*
        Seg seg0;
        seg0 = memory->main_mem[0];
        */
        /*
        for (int i = 0; i < fsize/4; i++) {
                printf("%u\n", seg0[i]);
        }
        */
        while (1) {
                cmd = (memory->main_mem[0])[memory->pcount++];
                instr_array[shiftr(cmd, 28)](memory, cmd);
        }
        free_memory(memory);
        return 0;
}
