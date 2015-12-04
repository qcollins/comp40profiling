/*
 * um_methods.c
 * Dan Meyer and Quinn Collins
 * 11/24/2015
 * Homework 7: Profiling
 * implementation of all 15 UM assembler instructions. These instructions are
 * stored in a method suite that is put into an array. Also defines struct
 * Three_regs which holds the values of 3 registers, to simplify getting and
 * storing registers for 3-register functions.  
 */
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "memory_manager.h"
#include <malloc.h>
#include "bitpack_inline.h"
#include "uint_stack.h"

#define MAXVAL 4294967296
#define NUMFUNCS 15

typedef struct Three_regs {
        uint32_t *a;
        uint32_t *b;
        uint32_t *c;
} Three_regs;

/* Gets pointers to three registers and stores them in a Three_regs struct. 
 * Used in each three-register function. 
 */
static inline Three_regs get_three_regs(unsigned cw)
{        
        Three_regs tr = {
                (memory.regs +  bitpack_getu(cw, 3, 6)),
                (memory.regs +  bitpack_getu(cw, 3, 3)),
                (memory.regs +  bitpack_getu(cw, 3, 0))
        };
        return tr;
}

/* conditional move: if $r[c] != 0, $r[a] := $r[b] */
static inline void CMOV(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        if (*tr.c != 0) {
                *tr.a = *tr.b;
        }
}

/* segmented load: $r[a] := $m[$r[b]$r[c]] */ 
static inline void SLOAD(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        *tr.a = (memory.main_mem[*tr.b])[*tr.c];
}

/* segmented store: $m[$r[a]$r[b]] := $r[c] */
static inline void SSTORE(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        (memory.main_mem)[*tr.a][*tr.b] = *tr.c;
}

/* addition: $r[a] := ($r[b] + $r[c]) % 2^32 */
static inline void ADD(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        *tr.a = (*tr.b + *tr.c) % MAXVAL;
}

/* multiplication: $r[a] := ($r[b] * $r[c]) % 2^32 */
static inline void MULT(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        *tr.a = ((*tr.b) * (*tr.c) ) % MAXVAL;
}

/* division: $r[a] := floor($r[b] / $r[c]) */
static inline void DIV(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        *tr.a = *tr.b / *tr.c;
}

/* bitwise NAND: $r[a] := !($r[b] & $r[c]) */
static inline void NAND(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        *tr.a = ~((*tr.b) & (*tr.c));
}

/* halt: computation stops */
static inline void HALT(unsigned cw)
{
        (void)cw;
        free_memory();
        exit(0);
}

/*
*/
static inline void expand_mem()
{
        Seg *main_mem = calloc(memory.mem_size * 2, 8);
        for (unsigned i = 0; i < memory.mem_size; i++)
                main_mem[i] = memory.main_mem[i];
        memory.mem_size *= 2;
        Seg *temp = memory.main_mem; 
        memory.main_mem = main_mem;
        free(temp);
}


/* map segment: $r[b] is given the segment identifier. New segment is
 * initialized with $r[c] words */
static inline void MAP(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        unsigned seg_index = 0;
        Seg new_seg = calloc(*tr.c, REGSIZE);
        if (stack_empty(memory.reuse_segs) != 1) {
                seg_index = stack_pop(memory.reuse_segs);
                memory.main_mem[seg_index] = new_seg;
        }
        else {
                if (memory.hi_seg >= memory.mem_size) {
                        expand_mem();
                }
                memory.main_mem[memory.hi_seg] = new_seg;
                seg_index = memory.hi_seg;
                memory.hi_seg++;
        }
        *tr.b = seg_index;
}

/* unmap segment: segment at $r[c] is unmapped. Future map instructions may
 * reuse the identifier */
static inline void UNMAP(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        uint32_t rc = *tr.c;
        Seg cur_seg;
        cur_seg = memory.main_mem[rc];
        memory.main_mem[rc] = NULL;
        stack_push(memory.reuse_segs, rc);
        free(cur_seg);
}

/* output: $r[c] is displayed to I/O. Only values 0-255 permitted */ 
static inline void OUTPUT(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        int output = *tr.c;
        assert(output < 256);
        putchar(output);
}

/* input: UM waits for input from I/O and puts it in $r[c]. only values 0-255
 * permitted. if end of input, fill $r[c] with 1s */
static inline void INPUT(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        int input  = getchar();
        *tr.c = input;
} 

/* load program: $m[$r[b]] is duplicated and moved to $m[0]. program counter is
 * set to $m[0][$r[c]]. */
static inline void LOADP(unsigned cw)
{
        Three_regs tr = get_three_regs(cw);
        uint32_t rb = *tr.b;
        if(rb != 0) {
                free(memory.main_mem[0]);
                memory.main_mem[0] = memory.main_mem[rb];
                memory.news0 = 1;
        }
        memory.pcount = *tr.c;
}

/* load value: loads value into specified register */
static inline void LOADV(unsigned cw)
{
        memory.regs[bitpack_getu(cw, 3, 25)] = bitpack_getu(cw, 25, 0);
}

typedef void (*cmd_ptr)(unsigned cw);
cmd_ptr instr_array[NUMFUNCS] = {CMOV, SLOAD, SSTORE, ADD, MULT, DIV,
                                 NAND, HALT, MAP, UNMAP, OUTPUT, INPUT,
                                 LOADP, LOADV};
