#include <stdio.h>

#define CMOV(a,b,c) printf("CMOV %d %d %d\n", a, b, c)
// SLOAD: $ra = $m[$rb][$rc]
#define SLOAD(a,b,c) printf("SLOAD %d %d %d\n", a, b, c)
// SSTORE: $m[$ra][$rb] = $rc
#define SSTORE(a,b,c) printf("SSTORE %d %d %d\n", a, b, c)
#define ADD(a,b,c) printf("ADD %d %d %d\n", a, b, c)
#define MULT(a,b,c) printf("MULT %d %d %d\n", a, b, c)
#define DIV(a,b,c) printf("DIV %d %d %d\n", a, b, c)
#define NAND(a,b,c) printf("NAND %d %d %d\n", a, b, c)
#define HALT printf("HALT 0 0 0\n")
#define MAP(a, len) printf("MAP 0 %d %d\n", a, len)
#define UNMAP(a) printf("UNMAP 0 0 %d\n", a)
#define OUTPUT(a) printf("OUTPUT 0 0 %d\n", a)
#define INPUT(a) printf("INPUT 0 0 %d\n", a)
#define LOADP(a,b) printf("LOADP 0 %d %d\n", a, b)
#define LOADV(a,b) printf("LOADV %d %d\n", a, b)
void write_program()
{
        LOADV(0, 8);
        MAP(2, 0);
        LOADV(0, 0);
        LOADV(1, 1);
        for (int i = 0; i < 8; i++) {
                LOADV(0, i+1);
                LOADV(1, i);
                SSTORE(2, 1, 0);
        }
        for (int i = 0; i < 7; i++) {
                LOADV(1, i);
                SLOAD(0, 2, 1);
                OUTPUT(0);
        }
        /*
        */
}

int main()
{
        write_program();
        HALT;
        return 0;
}
