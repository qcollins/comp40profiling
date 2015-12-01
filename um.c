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
#include "bitpack.h"
#include <time.h>

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
        unsigned len = UArray_length(seg0);
        while (memory->pcount < len) {
                if (opcode == 12) {
                        seg0 = (Seg)Seq_get(memory->main_mem, 0);
                        len = UArray_length(seg0);
                }
                cmd = *((unsigned *)UArray_at(seg0, memory->pcount)); 
                opcode = Bitpack_getu(cmd, 4, 28);
                instr_array[opcode](memory, cmd);
                if (opcode != 12) {
                        memory->pcount++;
                }
        }
        free_memory(memory);
        return 0;
}
