/*
 * umlab.c
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Code from lab 10 assignment that gets used to gnerate um binary files from
 * plain text instructions in tgen.c
 */

#include <stdint.h>
#include "bitpack.h"
#include <stdio.h>
#include "seq.h"
#include <assert.h>

typedef uint32_t Um_instruction;
typedef uint32_t Um_opcode;
enum opcodes {MOV, SLOAD, SSTORE, ADD, MULT, DIV, NAND, HALT, MAP, UMAP, OUT, 
              IN, LOADP, LOADV};

static inline Um_instruction three_register(Um_opcode op,
                                            int ra, 
                                            int rb, 
                                            int rc);
static inline Um_instruction loadval(unsigned ra, unsigned val);
static inline Um_instruction output(unsigned rc);
static inline Um_instruction loadprogram(unsigned rb, unsigned rc);

static inline Um_instruction halt(void)
{
	return three_register(HALT, 0, 0, 0);
}

static inline  Um_instruction three_register(Um_opcode op, 
                                             int ra, 
                                             int rb, 
                                             int rc)
{
        Um_instruction cword = 0;
        cword = Bitpack_newu(cword, 3, 0, rc);
        cword = Bitpack_newu(cword, 3, 3, rb);
        cword = Bitpack_newu(cword, 3, 6, ra);
        cword = Bitpack_newu(cword, 4, 28, op);
        return cword;
}

static inline Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction cword = 0;
        cword = Bitpack_newu(cword, 25, 0, val);
        cword = Bitpack_newu(cword, 3, 25, ra);
        cword = Bitpack_newu(cword, 4, 28, LOADV);
        return cword;
}

static inline Um_instruction output(unsigned rc)
{
        return three_register(OUT, 0, 0, rc);
}

static inline Um_instruction loadprogram(unsigned rb, unsigned rc)
{
        return three_register(LOADP, 0, rb, rc);
}


enum regs { r0 = 0, r1, r2, r3, r4, r5, r6, r7 };

extern void Um_write_sequence(FILE *output, Seq_T stream);

static inline void emit(Seq_T stream, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_addhi(stream, (void *)(uintptr_t) inst);
} 
static inline Um_instruction get_inst(Seq_T stream, int i)
{
	assert(sizeof(Um_instruction) <= sizeof(uintptr_t));
	return (Um_instruction) (uintptr_t) (Seq_get(stream, i));
}

static inline void put_inst(Seq_T stream, int i, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_put(stream, i, (void *)(uintptr_t) inst);
}

void emit_halt_test(Seq_T stream)
{
	emit(stream, halt());
	emit(stream, loadval(r1, 'B'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'a'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'd'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '!'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '\n'));
	emit(stream, output(r1));
}

static void add_label(Seq_T stream, int location_to_patch, int label_value)
{
	Um_instruction inst = get_inst(stream, location_to_patch);
	unsigned k = Bitpack_getu(inst, 25, 0);
	inst = Bitpack_newu(inst, 25, 0, label_value + k);
	put_inst(stream, location_to_patch, inst);
}

static void emit_out_string(Seq_T stream, const char *s, int aux_reg)
{
        int i = 0;
        while (s[i] != '\0') {
                emit(stream, loadval(aux_reg, (uint32_t)s[i]));
                emit(stream, output(aux_reg));
                i++;
        }
}

void emit_goto_test(Seq_T stream)
{
	int patch_L = Seq_length(stream);
	emit(stream, loadval(r7, 0));	     /* will be patched to 'r7 := L' */
	emit(stream, loadprogram(r0, r7));   /* should goto label L          */
	emit_out_string(stream, "GOTO failed.\n", r1);
	emit(stream, halt());
	/* define 'L' to be here */
	add_label(stream, patch_L, Seq_length(stream));	
	emit_out_string(stream, "GOTO passed.\n", r1);
	emit(stream, halt());
}
