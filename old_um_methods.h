/*
 * Um_methods.h
 * Dan Meyer and Quinn Collins
 * 11/24/2015
 * contains UM assembler instructions to be used in method suite
 */
#ifndef UM_METHODS_INCLUDED 
#define UM_METHODS_INCLUDED

#include "memory_manager.h"
#define NUMFUNCS 15

void CMOV(Mem memory, unsigned cw);
void SLOAD(Mem memory, unsigned cw);
void SSTORE(Mem memory, unsigned cw);
void ADD(Mem memory, unsigned cw);
void MULT(Mem memory, unsigned cw);
void DIV(Mem memory, unsigned cw);
void NAND(Mem memory, unsigned cw);
void HALT(Mem memory, unsigned cw);
void MAP(Mem memory, unsigned cw);
void UNMAP(Mem memory, unsigned cw);
void OUTPUT(Mem memory, unsigned cw);
void INPUT(Mem memory, unsigned cw);
void LOADP(Mem memory, unsigned cw);
void LOADV(Mem memory, unsigned cw);
typedef void (*cmd_ptr)(Mem memory, unsigned cw);
cmd_ptr instr_array[NUMFUNCS];

#endif
