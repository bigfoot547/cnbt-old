#include "nbt_endian.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* for abort(3) */

int nbt_endian_invalid(void) {
    fprintf(stderr, "Unsupported endianness %u :(\n", nbt_endian());
    abort();
    return 0;
}

int nbt_endian(void) {
    uint32_t t = 0x02999801u;
    int endian = (int)*((uint8_t *)&t);
    if (endian == _NBT_ENDIAN_WTF) return nbt_endian_invalid();

    return endian;
}

/* below functions are optimized by gcc into just bswap or bitwise rotate */
uint64_t nbt_endian_swap_u64(uint64_t in) {
    return (in >> 56)
        | (in & 0x00FF000000000000ull) >> 40
        | (in & 0x0000FF0000000000ull) >> 24
        | (in & 0x000000FF00000000ull) >> 8
        | (in & 0x00000000FF000000)    << 8
        | (in & 0x0000000000FF0000)    << 24
        | (in & 0x000000000000FF00)    << 40
        | (in << 56);
}

int64_t nbt_endian_swap_s64(int64_t in) {
    return ((in >> 56) & 0xFF) /* remove sign-extension */
        | (in & 0x00FF000000000000ull) >> 40
        | (in & 0x0000FF0000000000ull) >> 24
        | (in & 0x000000FF00000000ull) >> 8
        | (in & 0x00000000FF000000)    << 8
        | (in & 0x0000000000FF0000)    << 24
        | (in & 0x000000000000FF00)    << 40
        | (in << 56);
}

uint32_t nbt_endian_swap_u32(uint32_t in) {
    return (in >> 24)
        | (in & 0x00FF0000) >> 8
        | (in & 0x0000FF00) << 8
        | (in << 24);
}

int32_t nbt_endian_swap_s32(int32_t in) {
    return ((in >> 24) & 0xFF) /* remove sign-extension */
        | (in & 0x00FF0000) >> 8
        | (in & 0x0000FF00) << 8
        | (in << 24);
}

uint16_t nbt_endian_swap_u16(uint16_t in) {
    return (in >> 8) | (in << 8);
}

int16_t nbt_endian_swap_s16(int16_t in) {
    return ((in >> 8) & 0xFF) | (in << 8);
}

#define SWAP_FUNC(_ct, _bits)                                    \
_ct nbt_endian_h2be_ ## _bits(_ct in) {                          \
    if (NBT_ENDIAN_LITTLE) return nbt_endian_swap_ ## _bits(in); \
    else if (NBT_ENDIAN_BIG) return in;                          \
    return nbt_endian_invalid();                                 \
}

SWAP_FUNC(uint64_t, u64)
SWAP_FUNC(uint32_t, u32)
SWAP_FUNC(uint16_t, u16)

SWAP_FUNC(int64_t, s64)
SWAP_FUNC(int32_t, s32)
SWAP_FUNC(int16_t, s16)

#undef SWAP_FUNC
