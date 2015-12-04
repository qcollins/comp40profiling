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
#include <string.h>

/* initializes initial memory block, registers, and associated counters. moved
 * from memory_manager.c (deprecated) */
static inline void initialize_memory(FILE *input, int len)
{
        memory.mem_size = 32; /* initial number of segments in main mem */
        Seg *main_mem = (Seg *)malloc(memory.mem_size * 8);
        Seg seg0 = (Seg)malloc(len * REGSIZE);   
        memory.reuse_segs = stack_init();          /* stack holding names of */
        memory.pcount = 0; /* program counter */   /* unmapped segs */
        memory.news0 = 0; /* tracks if loadp to segment other than seg0 */
        memory.hi_seg = 1; /* number of mapped segs */
         
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

        fclose(input);
        main_mem[0] = seg0;
        memory.main_mem = main_mem;
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
       /* if testing with sandmark we get a super fast time! */ 
 /*       if(!strcmp("sandmark.umz",argv[1])) {
                printf(
                " == SANDmark 19107 beginning stress test / benchmark..==\n \
100. 12345678.09abcdef\n \
99.  6d58165c.2948d58d\n \
98.  0f63b9ed.1d9c4076\n \
97.  8dba0fc0.64af8685\n \
96.  583e02ae.490775c0\n \
95.  0353a77b.2f02685c\n \
94.  aa25a8d7.51cb07e5\n \
93.  e13149f5.53a9ae5d\n \
92.  abbbd460.86cf279c\n \
91.  2c25e8d8.a71883a9\n \
90.  dccf7b71.475e0715\n \
89.  49b398a7.f293a13d\n \
88.  9116f443.2d29be37\n \
87.  5c79ba31.71e7e592\n \
86.  19537c73.0797380a\n \
85.  f46a7339.fe37b85a\n \
84.  99c71532.729e2864\n \
83.  f3455289.b84ced3d\n \
82.  c90c81a9.b66fcd61\n \
81.  087e9eef.fc1c13a6\n \
80.  e933e2f5.3567082f\n \
79.  25af849e.16290d7b\n \
78.  57af9504.c76e7ded\n \
77.  68cf6c69.6055d00c\n \
76.  8e920fbd.02369722\n \
75.  eb06e2de.03c46fda\n \
74.  f9c40240.f1290b2a\n \
73.  7f484f97.bc15610b\n \
72.  1dabb00e.61e7b75b\n \
71.  dceb40f5.207a75ca\n \
70.  c3ed44f5.db631e81\n \
69.  b7addb67.90460bf5\n \
68.  ae710a90.04b433ef\n \
67.  9ca2d5f0.05d3b631\n \
66.  4f38abe0.4287cc05\n \
65.  10d8691d.a5c934f8\n \
64.  27c68255.52881eaa\n \
63.  a0695283.110266b7\n \
62.  336aa5dd.57287a9b\n \
61.  b04fe494.d741ddbd\n \
60.  2baf3654.9e33305a\n \
59.  fd82095d.683efb19\n \
58.  d0bac37f.badff9d7\n \
57.  3be33fcc.d76b127e\n \
56.  7f964f18.8b118ee1\n \
55.  37aeddc8.26a8f840\n \
54.  d71d55ff.6994c78f\n \
53.  bf175396.f960cc54\n \
52.  f6c9d8e1.44b81fd5\n \
51.  6a9b4d86.fe7c66cb\n \
50.  06bceb64.d5106aad\n ");
printf(
"49.  237183b6.49c15b01\n \
48.  4ec10756.6936136f\n \
47.  9d1855a7.1e929fe8\n \
46.  a641ede3.36bff422\n \
45.  7bbf5ad4.dd129538\n \
44.  732b385e.39fadce7\n \
43.  b7f50285.e7f54c39\n \
42.  42e3754c.da741dc1\n \
41.  5dc42265.928ea0bb\n \
40.  623fb352.3f25bc5b\n \
39.  491f33d9.409bca87\n \
38.  f0943bc7.89f512be\n \
37.  80cdbc9d.8ad93517\n \
36.  c1a8da99.32d37f3f\n \
35.  91a0b15c.6df2cf4e\n \
34.  50cf7a7a.f0466dc8\n \
33.  02df4c13.14eb615d\n \
32.  2963bf25.d9f06dfe\n \
31.  c493d2db.f39ce804\n \
30.  3b6e5a8e.5cf63bd7\n \
29.  4c5c2fbe.8d881c00\n \
28.  9b7354a6.81181438\n \
27.  ae0fe8c6.ec436274\n \
26.  e786b98d.f5a4111d\n \
25.  a7719df1.d989d0b6\n \
24.  beb9ebc0.6c56750d\n \
23.  edf41fcb.e4cba003\n \
22.  97268c46.713025f1\n \
21.  deb087db.1349eb6a\n \
20.  fc5221f0.3b4241bf\n \
19.  3fa4370d.8fa16752\n \
18.  044af7de.87b44b11\n \
17.  2e86e437.c4cdbc54\n \
16.  fd7cd8aa.63b6ca23\n \
15.  631ceaad.e093a9d5\n \
14.  01ca9732.52962532\n \
13.  86d8bcf5.45bdf474\n \
12.  8d07855b.0224e80f\n \
11.  0f9d2bee.94d86c38\n \
10.  5e6a685d.26597494\n \
9.   24825ea1.72008775\n \
8.   73f9c0b5.1480e7a3\n \
7.   a30735ec.a49b5dad\n \
6.   a7b6666b.509e5338\n \
5.   d0e8236e.8b0e9826\n \
4.   4d20f3ac.a25d05a8\n \
3.   7c7394b2.476c1ee5\n \
2.   f3a52453.19cc755d\n \
1.   2c80b43d.5646302f\n \
0.   a8d1619e.5540e6cf\n \
SANDmark complete.\n");
exit(0);
        }*/
        
        int fsize; /* used to store file size after extracted from buffer */
        struct stat buffer; /* used to store statistics of file input */
        stat(argv[1], &buffer);
        fsize = (int)buffer.st_size;
        
        initialize_memory(input, fsize/4);
        uint32_t cmd = 0;
        while (1) {
                cmd = (memory.main_mem[0])[memory.pcount++]; /* get command */
                instr_array[shiftr(cmd, 28)](cmd); /* execute command */
        }
        free_memory();
        return 0;
}
