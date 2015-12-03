/*
 * um_methods.c
 * Dan Meyer and Quinn Collins
 * 11/24/2015
 * implementation of all 15 UM assembler instructions. These instructions are
 * stored in a method suite that is put into an array. Also defines struct
 * Three_regs which holds the values of 3 registers, to simplify getting and
 * storing registers for 3-register functions.  
 */
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "um_methods.h"
#include "memory_manager.h"
#include <malloc.h>
#include "bitpack_inline.h"

#define MAXVAL 4294967296
#define REGSIZE 4
#define NUMFUNCS 15

/* method suite which is stored in array for easy use of UM assembler
 * instructions in memory_manager and um.c */ 
cmd_ptr instr_array[NUMFUNCS] = {CMOV, SLOAD, SSTORE, ADD, MULT, DIV,
                                 NAND, HALT, MAP, UNMAP, OUTPUT, INPUT,
                                 LOADP, LOADV};

typedef struct Three_regs {
        uint32_t *a;
        uint32_t *b;
        uint32_t *c;
} Three_regs;

/* Gets pointers to three registers and stores them in a Three_regs struct. 
 * Used in each three-register function. 
 */


static inline Three_regs get_three_regs(Mem memory, unsigned cw)
{        
        Three_regs tr = {
                (memory->regs +  bitpack_getu(cw, 3, 6)),
                (memory->regs +  bitpack_getu(cw, 3, 3)),
                (memory->regs +  bitpack_getu(cw, 3, 0))
        };
        return tr;
}

/* conditional move: if $r[c] != 0, $r[a] := $r[b] */
void CMOV(Mem memory, unsigned cw)
{
        // printf("CMOV\n");
        Three_regs tr = get_three_regs(memory, cw);
        if (*tr.c != 0) {
                *tr.a = *tr.b;
        }
}

/* segmented load: $r[a] := $m[$r[b]$r[c]] */ 
void SLOAD(Mem memory, unsigned cw)
{
        // printf("SLOAD\n");
        Three_regs tr = get_three_regs(memory, cw);
        Seg cur_seg;
        //printf("trb = %u\n", *tr.b);
        //cur_seg = (Seg)Seq_get(memory->main_mem, *tr.b);
        cur_seg = memory->main_mem[*tr.b];
        *tr.a = cur_seg[*tr.c];
        //*tr.a = *((unsigned*)UArray_at(cur_seg, *tr.c));
}

/* segmented store: $m[$r[a]$r[b]] := $r[c] */
void SSTORE(Mem memory, unsigned cw)
{
        // printf("SSTORE\n");
        Three_regs tr = get_three_regs(memory, cw);
        Seg cur_seg;
        cur_seg = memory->main_mem[*tr.a];
        //cur_seg = (Seg)Seq_get(memory->main_mem, *tr.a);
        //*((unsigned*)UArray_at(cur_seg, *tr.b)) = *tr.c;
        cur_seg[*tr.b] = *tr.c;
}

/* addition: $r[a] := ($r[b] + $r[c]) % 2^32 */
void ADD(Mem memory, unsigned cw)
{
        // printf("ADD\n");
        Three_regs tr = get_three_regs(memory, cw);
        *tr.a = (*tr.b + *tr.c) % MAXVAL;
}

/* multiplication: $r[a] := ($r[b] * $r[c]) % 2^32 */
void MULT(Mem memory, unsigned cw)
{
        // printf("MULT\n");
        Three_regs tr = get_three_regs(memory, cw);
        *tr.a = ((*tr.b) * (*tr.c) ) % MAXVAL;
}

/* division: $r[a] := floor($r[b] / $r[c]) */
void DIV(Mem memory, unsigned cw)
{
        // printf("DIV\n");
        Three_regs tr = get_three_regs(memory, cw);
        *tr.a = *tr.b / *tr.c;
}

/* bitwise NAND: $r[a] := !($r[b] & $r[c]) */
void NAND(Mem memory, unsigned cw)
{
        // printf("NAND\n");
        Three_regs tr = get_three_regs(memory, cw);
        *tr.a = ~((*tr.b) & (*tr.c));
}

/* halt: computation stops */
void HALT(Mem memory, unsigned cw)
{
        // printf("HALT\n");
        (void)cw;
        free_memory(memory);
        exit(0);
}

/*
static inline void expand_mem(Mem memory) 
{
        //printf("num calls = %u\n", memory->hi_seg);
        memory->mem_size *= 2;
        Seg *main_mem = calloc(memory->mem_size, 8);
        for (unsigned i = 0; i < memory->mem_size/2; i++)
                main_mem[i] = memory->main_mem[i];
        Seg *temp = memory->main_mem; 
        memory->main_mem = main_mem;
        free(temp);
}
*/


/* map segment: $r[b] is given the segment identifier. New segment is
 * initialized with $r[c] words */
void MAP(Mem memory, unsigned cw)
{
        // printf("MAP\n");
        Three_regs tr = get_three_regs(memory, cw);
        unsigned seg_index = 0;
        Seg new_seg = calloc(*tr.c, REGSIZE);
        if (Stack_empty(memory->free_regs) != 1) {
                seg_index = (unsigned)(uintptr_t)Stack_pop(memory->free_regs);
                memory->main_mem[seg_index] = new_seg;
        }
        else {
                if (memory->hi_seg >= memory->mem_size) {
                        memory->main_mem = realloc(memory->main_mem, 
                                                   8 * (memory->mem_size * 2));
                        
                        memory->mem_size *= 2;
                        //expand_mem(memory);
                }
                memory->main_mem[memory->hi_seg] = new_seg;
                seg_index = memory->hi_seg;
                memory->hi_seg++;
        }
        *tr.b = seg_index;
}

/* unmap segment: segment at $r[c] is unmapped. Future map instructions may
 * reuse the identifier */
void UNMAP(Mem memory, unsigned cw)
{
        // printf("UNMAP\n");
        Three_regs tr = get_three_regs(memory, cw);
        uint32_t rc = *tr.c;
        Seg *main_mem = memory->main_mem;
        Seg cur_seg;
        cur_seg = main_mem[rc];
        main_mem[rc] = NULL;
        Stack_push(memory->free_regs, 
                   (void*)(uintptr_t)(rc));
        free(cur_seg);
}

/* output: $r[c] is displayed to I/O. Only values 0-255 permitted */
void OUTPUT(Mem memory, unsigned cw)
{
        // printf("OUTPUT\n");
        Three_regs tr = get_three_regs(memory, cw);
        int output = *tr.c;
        assert(output < 256);
        putchar(output);
}

/* input: UM waits for input from I/O and puts it in $r[c]. only values 0-255
 * permitted. if end of input, fill $r[c] with 1s */
void INPUT(Mem memory, unsigned cw)
{
        // printf("INPUT\n");
        Three_regs tr = get_three_regs(memory, cw);
        int input  = getchar();
        *tr.c = input;
} 

/*
static inline Seg seg_cpy(Seg s1, Seg s2) 
{
        unsigned len = malloc_usable_size(s1)/4;
        s2 = calloc(len, sizeof(s1[0]));
        for (unsigned i = 0; i < len; i ++)
                s2[i] = s1[i];
        return s2;
}
*/

/* load program: $m[$r[b]] is duplicated and moved to $m[0]. program counter is
 * set to $m[0][$r[c]]. */
void LOADP(Mem memory, unsigned cw)
{
        // printf("LOADP\n");
        Three_regs tr = get_three_regs(memory, cw);
        Seg *main_mem = memory->main_mem;
        uint32_t rb = *tr.b;
        Seg new_seg = main_mem[rb];
        //new_seg = seg_cpy(main_mem[rb], new_seg);
        //Seg old_seg = main_mem[0];
        if (rb != 0) {
                // free(main_mem[0]);
                //main_mem[rb] = NULL;
                memory->news0 = 1;
        }
        main_mem[0] = new_seg;
        memory->pcount = *tr.c;
}

/* load value: loads value into specified register */
void LOADV(Mem memory, unsigned cw)
{
        // printf("LOADV\n");
        unsigned val = bitpack_getu(cw, 25, 0);
        int reg = bitpack_getu(cw, 3, 25);
        memory->regs[reg] = val;
}
