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
#include "memory_manager.h"
#include <malloc.h>
#include "bitpack_inline.h"
#include "uint_stack.h"

#define MAXVAL 4294967296
#define NUMFUNCS 15

/* method suite which is stored in array for easy use of UM assembler
 * instructions in memory_manager and um.c */ 

typedef struct Three_regs {
        uint32_t *a;
        uint32_t *b;
        uint32_t *c;
} Three_regs;

/* Gets pointers to three registers and stores them in a Three_regs struct. 
 * Used in each three-register function. 
 */


/*
*/
static inline Three_regs get_three_regs(Inst cmd)
{        
        Three_regs tr = {
                (memory.regs +  cmd.ra),
                (memory.regs +  cmd.rb),
                (memory.regs +  cmd.rc)
        };
        return tr;
}

/* conditional move: if $r[c] != 0, $r[a] := $r[b] */
static inline void CMOV(Inst cmd)
{
        // printf("CMOV\n");
        Three_regs tr = get_three_regs(cmd);
        if (*tr.c != 0) {
                *tr.a = *tr.b;
        }
}

/* segmented load: $r[a] := $m[$r[b]$r[c]] */ 
static inline void SLOAD(Inst cmd)
{
        Three_regs tr = get_three_regs(cmd);
        *tr.a = (memory.main_mem[*tr.b])[*tr.c];
}

/* segmented store: $m[$r[a]$r[b]] := $r[c] */
static inline void SSTORE(Inst cmd)
{
        // printf("SSTORE\n");
        Three_regs tr = get_three_regs(cmd);
        (memory.main_mem)[*tr.a][*tr.b] = *tr.c;
}

/* addition: $r[a] := ($r[b] + $r[c]) % 2^32 */
static inline void ADD(Inst cmd)
{
        // printf("ADD\n");
        Three_regs tr = get_three_regs(cmd);
        *tr.a = (*tr.b + *tr.c) % MAXVAL;
}

/* multiplication: $r[a] := ($r[b] * $r[c]) % 2^32 */
static inline void MULT(Inst cmd)
{
        // printf("MULT\n");
        Three_regs tr = get_three_regs(cmd);
        *tr.a = ((*tr.b) * (*tr.c) ) % MAXVAL;
}

/* division: $r[a] := floor($r[b] / $r[c]) */
static inline void DIV(Inst cmd)
{
        // printf("DIV\n");
        Three_regs tr = get_three_regs(cmd);
        *tr.a = *tr.b / *tr.c;
}

/* bitwise NAND: $r[a] := !($r[b] & $r[c]) */
static inline void NAND(Inst cmd)
{
        // printf("NAND\n");
        Three_regs tr = get_three_regs(cmd);
        *tr.a = ~((*tr.b) & (*tr.c));
}

/* halt: computation stops */
static inline void HALT(Inst cmd)
{
        // printf("HALT\n");
        (void)cmd;
        Seg *main_mem = memory.main_mem;
        Seg segment = main_mem[0];
        if (memory.news0 == 0)
                free(segment);
        for (unsigned i = 1; i < memory.hi_seg; i++) {
                if (main_mem[i] != 0)
                        free(main_mem[i]);
        }
        stack_free(memory.reuse_segs);
        free(main_mem);
        exit(0);
}

/*
*/
static inline void expand_mem()
{
        Seg *main_mem = calloc(memory.mem_size * 2, 8);
        unsigned size = memory.mem_size;
        for (unsigned i = 0; i < size; i++)
                main_mem[i] = memory.main_mem[i];
        memory.mem_size *= 2;
        Seg *temp = memory.main_mem; 
        memory.main_mem = main_mem;
        free(temp);
}


/* map segment: $r[b] is given the segment identifier. New segment is
 * initialized with $r[c] words */
static inline void MAP(Inst cmd)
{
        Three_regs tr = get_three_regs(cmd);
        unsigned seg_index = 0;
        Seg new_seg = calloc(*tr.c, REGSIZE);
        if (stack_empty(memory.reuse_segs) != 1) {
                seg_index = stack_pop(memory.reuse_segs);
                memory.main_mem[seg_index] = new_seg;
        }
        else {
                if (memory.hi_seg >= memory.mem_size) {
                        /*
                        memory->main_mem = realloc(memory->main_mem, 
                                                   8 * (memory->mem_size * 2));
                        
                        memory->mem_size *= 2;
                        */
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
static inline void UNMAP(Inst cmd)
{
        // printf("UNMAP\n");
        Three_regs tr = get_three_regs(cmd);
        uint32_t rc = *tr.c;
        //Seg *main_mem = memory.main_mem;
        Seg cur_seg;
        cur_seg = memory.main_mem[rc];
        memory.main_mem[rc] = NULL;
        stack_push(memory.reuse_segs, rc);
        free(cur_seg);
}

/* output: $r[c] is displayed to I/O. Only values 0-255 permitted */ 
static inline void OUTPUT(Inst cmd)
{
        // printf("OUTPUT\n");
        Three_regs tr = get_three_regs(cmd);
        int output = *tr.c;
        assert(output < 256);
        putchar(output);
}

/* input: UM waits for input from I/O and puts it in $r[c]. only values 0-255
 * permitted. if end of input, fill $r[c] with 1s */
static inline void INPUT(Inst cmd)
{
        // printf("INPUT\n");
        Three_regs tr = get_three_regs(cmd);
        int input  = getchar();
        *tr.c = input;
} 

static inline Seg seg_cpy(Seg s1, Seg s2) 
{
        unsigned len = malloc_usable_size(s1)/4;
        s2 = calloc(len, sizeof(s1[0]));
        free(cmds);
        cmds = malloc(sizeof(Inst)*len);
        for (unsigned i = 0; i < len; i ++) {
                s2[i] = s1[i];
                cmds[i].opcode = shiftr(s2[i], 28);
                printf("opcode: %u\n", cmds[i].opcode);
                if (cmds[i].opcode != 13) {
                        cmds[i].ra = bitpack_getu(s2[i], 3, 6);
                        cmds[i].rb = bitpack_getu(s2[i], 3, 3);
                        cmds[i].rc = bitpack_getu(s2[i], 3, 0);
                } else {
                        cmds[i].ra = bitpack_getu(s2[i], 3, 25);
                        cmds[i].rb = bitpack_getu(s2[i], 25, 0);
                }
        }
        return s2;
}

/* load program: $m[$r[b]] is duplicated and moved to $m[0]. program counter is
 * set to $m[0][$r[c]]. */
static inline void LOADP(Inst cmd)
{
        // printf("LOADP\n");
        Three_regs tr = get_three_regs(cmd);
        uint32_t rb = *tr.b;
        if (rb != 0) {
                memory.main_mem[0] = seg_cpy(memory.main_mem[rb], 
                                             memory.main_mem[0]);
                memory.news0 = 1;
        }
        //memory.main_mem[0] = new_seg;
        memory.pcount = *tr.c;
        /*
        printf("tr.c = %u\n", *tr.c);
        printf("rc = %u\n", memory.regs[cmd.rc]);
        */
}

/* load value: loads value into specified register */
static inline void LOADV(Inst cmd)
{
        memory.regs[cmd.ra] = cmd.rb;
}

typedef void (*cmd_ptr)(Inst cmd);
cmd_ptr instr_array[NUMFUNCS] = {CMOV, SLOAD, SSTORE, ADD, MULT, DIV,
                                 NAND, HALT, MAP, UNMAP, OUTPUT, INPUT,
                                 LOADP, LOADV};
