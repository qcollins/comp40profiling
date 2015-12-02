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
