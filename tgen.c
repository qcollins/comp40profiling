/*
 * tgen.c
 * Dan Meyer and Quinn Collins
 * 11/24/15
 * Generates .um binary files from simple text files containing assembly
 * instructions.
 * Uses umlab.c as a wrapper for emitting instructions to stdout.
 */
#define _GNU_SOURCE
#include "umlab.c"
#include <stdlib.h>
#include <string.h>

Seq_T load_seq(FILE *input, Seq_T stream)
{
        char *line = NULL;
        size_t len = 0;
        char opcode [7];
        int ra = 0, rb = 0, rc = 0;
        while (getline(&line, &len, input) != -1) {
                sscanf(line, "%s %d %d %d", opcode,&ra,&rb,&rc);
                if (strcmp(opcode, "CMOV") == 0) {
                        emit(stream, three_register(0, ra, rb, rc));
                }
                else if (strcmp(opcode, "SLOAD") == 0) {
                        emit(stream, three_register(1, ra, rb, rc));
                }
                else if (strcmp(opcode, "SSTORE") == 0) {
                        emit(stream, three_register(2, ra, rb, rc));
                }
                else if (strcmp(opcode, "ADD") == 0) {
                        emit(stream, three_register(3, ra, rb, rc));
                }
                else if (strcmp(opcode, "MULT") == 0) {
                        emit(stream, three_register(4, ra, rb, rc));
                }
                else if (strcmp(opcode, "DIV") == 0) {
                        emit(stream, three_register(5, ra, rb, rc));
                }
                else if (strcmp(opcode, "NAND") == 0) {
                        emit(stream, three_register(6, ra, rb, rc));
                }
                else if (strcmp(opcode, "HALT") == 0) {
                        emit(stream, halt());
                }
                else if (strcmp(opcode, "MAP") == 0) {
                        emit(stream, three_register(8, ra, rb, rc));
                }
                else if (strcmp(opcode, "UNMAP") == 0) {
                        emit(stream, three_register(9, ra, rb, rc));
                }
                else if (strcmp(opcode, "OUTPUT") == 0) {
                        emit(stream, output(rc));
                }
                else if (strcmp(opcode, "INPUT") == 0) {
                        emit(stream, three_register(11, 0, 0, rc));
                }
                else if (strcmp(opcode, "LOADP") == 0) {
                        emit(stream, three_register(12, ra, rb, rc));
                }
                else if (strcmp(opcode, "LOADV") == 0) {
                        emit(stream, loadval(ra, rb));
                }
                else {
                        assert(1);
                }
        }
        return stream;
}

void write_seq(Seq_T seq)
{
        int len = Seq_length(seq);
        unsigned inst = 0;
        for (int i = 0; i < len; i++) {
                inst = get_inst(seq,i);
                for (int j = 32; j >= 8; j-=8) {
                        putc(Bitpack_getu(inst, 8, j-8), stdout);
                }
        }
}

int main(int argc, char *argv[])
{
        (void)argc;
        FILE *input = fopen(argv[1], "r");
        Seq_T seq = Seq_new(0);
        seq = load_seq(input, seq);
        write_seq(seq);
        Seq_free(&seq);
        return 0;
}
