#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitpack.h"
#include "except.h"

#define SIZE 64

Except_T Bitpack_Overflow = {"Overflow packing bits"};

static inline uint64_t shiftl(uint64_t n, unsigned len)
{
        if (len >= SIZE)
                return (uint64_t)0;
        n <<= len;
        return n;
}

static inline uint64_t shiftr(uint64_t n, unsigned len)
{
        if (len >= SIZE)
                return (uint64_t)0;
        n >>= len;
        return n;
}

static inline uint64_t sshiftr(uint64_t n, unsigned len)
{
        if (len >= SIZE)
                return (uint64_t)0;
        int64_t output = n;
        output >>= len;
        return (uint64_t)output;
}


bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        n = shiftr(n, width);
        if (n == 0)
                return true;
        return false;
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
        n = sshiftr(n, width);
        if (n == 0 || n  == -1)
                return true;
        return false;
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        if(width+lsb > SIZE)
                RAISE(Bitpack_Overflow);
        if (width == 0)
                return 0;
        word = shiftl(word, SIZE-width-lsb);
        word = shiftr(word, SIZE-width);
        return word;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        int64_t output = word;
        if (width == 0)
                return 0;
        if(width+lsb > SIZE)
                RAISE(Bitpack_Overflow);
        output = shiftl(output, SIZE-width-lsb);
        output = sshiftr(output, SIZE-width);
        return output;
}

uint64_t Bitpack_news(uint64_t word, 
                      unsigned width, 
                      unsigned lsb, 
                      int64_t value)
{
        if (!Bitpack_fitss(value, width))
                RAISE(Bitpack_Overflow);
        uint64_t mask = 0;
        mask = ~mask;
        mask = shiftl(mask, SIZE-width);
        mask = shiftr(mask, SIZE-lsb-width);
        mask = ~mask;
        word &= mask;
        value = shiftl(value, SIZE-width);
        value = shiftr(value, SIZE-lsb-width);
        word |= value;
        return word;
}

uint64_t Bitpack_newu(uint64_t word, 
                      unsigned width, 
                      unsigned lsb, 
                      uint64_t value)
{
        if (!Bitpack_fitsu(value, width))
                RAISE(Bitpack_Overflow);
        uint64_t mask = 0;
        mask = ~mask;
        mask = shiftl(mask, SIZE-width);
        mask = shiftr(mask, SIZE-lsb-width);
        mask = ~mask;
        word &= mask;
        value = shiftl(value, lsb);
        word |= value;
        return word;
}
#undef SIZE
