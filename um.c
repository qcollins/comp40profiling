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
#include "seq.h"
#include "memory_manager.h"
#include "uarray.h"
#include <stdint.h>
#include "um_methods.h"
#include "bitpack_inline.h"
#include <time.h>

static inline unsigned array_len(unsigned *array)
{
        int len = sizeof(array)/sizeof(array[0]);
        return len;
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
        unsigned opcode = 0;
        unsigned cmd = 0;
        Seg seg0;
        seg0 = (Seg)Seq_get(memory->main_mem, 0);
        //unsigned len = array_len(seg0);
        //while (memory->pcount < len) {
        int count = 0;
        while (1) {
                cmd = seg0[memory->pcount];
                opcode = bitpack_getu(cmd, 4, 28);
                /*
                if (opcode == 7) {
                        printf("HALT!\n");
                }
                */
                //printf("opcode: %d\n", opcode);
                instr_array[opcode](memory, cmd);
                if (opcode == 12) {
                        seg0 = (Seg)Seq_get(memory->main_mem, 0);
                        //len = array_len(seg0);
                }
                if (opcode != 12)
                        memory->pcount++;
                count++;
                //printf("count: %d\n", count);
        }
        free_memory(memory);
        return 0;
}
