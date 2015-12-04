/*
 * bitpack_inline.h
 * By Daniel Meyer and Quinn Collins
 * Homework 7: Profiling
 * Slight modification to bitpack solutions provided for the solutions to
 * arith that makes use of static inline to improve performance and match the
 * data type to what we use in our um, uint32_t, and only includes functions
 * that we need.
 */
#ifndef BITPACK_INLINE_INCLUDED 
#define BITPACK_INLINE_INCLUDED
#include <stdint.h>

static inline uint32_t shiftl(uint32_t word, unsigned bits)
{
        if (bits == 32)
                return 0;
        return word << bits;
}
static inline uint32_t shiftr(uint32_t word, unsigned bits)
{
        if (bits == 32)
                return 0;
        return word >> bits;
}

static inline uint32_t bitpack_getu(uint32_t word, unsigned width, unsigned lsb)
{
        unsigned hi = lsb+width;
        return shiftr(shiftl(word, 32-hi), 32-width);
}

static inline uint32_t bitpack_newu(uint32_t word, unsigned width, unsigned lsb,
                      uint32_t value)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        return shiftl(shiftr(word, hi), hi)                 /* high part */
                | shiftr(shiftl(word, 32 - lsb), 32 - lsb)  /* low part  */
                | (value << lsb);                     /* new part  */
}
#endif
