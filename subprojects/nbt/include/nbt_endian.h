#ifndef LIBNBT_ENDIAN_H_INCLUDED
#define LIBNBT_ENDIAN_H_INCLUDED

#include <stdint.h>
#include "nbt_def.h"

/* TODO: replace all of this with something else */

int nbt_endian(void);

enum {
    _NBT_ENDIAN_LITTLE = 0x01,
    _NBT_ENDIAN_BIG = 0x02,
    _NBT_ENDIAN_WTF = 0x99,
};

#define NBT_ENDIAN_LITTLE (nbt_endian() == _NBT_ENDIAN_LITTLE)
#define NBT_ENDIAN_BIG    (nbt_endian() == _NBT_ENDIAN_BIG)
#define NBT_ENDIAN_WTF    (nbt_endian() == _NBT_ENDIAN_WTF)

uint64_t nbt_endian_h2be_u64(uint64_t in);
uint32_t nbt_endian_h2be_u32(uint32_t in);
uint16_t nbt_endian_h2be_u16(uint16_t in);

int64_t nbt_endian_h2be_s64(int64_t in);
int32_t nbt_endian_h2be_s32(int32_t in);
int16_t nbt_endian_h2be_s16(int16_t in);

#define nbt_endian_be2h_u64 nbt_endian_h2be_u64
#define nbt_endian_be2h_u32 nbt_endian_h2be_u32
#define nbt_endian_be2h_u16 nbt_endian_h2be_u16

#define nbt_endian_be2h_s64 nbt_endian_h2be_s64
#define nbt_endian_be2h_s32 nbt_endian_h2be_s32
#define nbt_endian_be2h_s16 nbt_endian_h2be_s16

/* with NBT lowercase type names */
#define nbt_endian_h2be_long(_in)  (nbt_endian_h2be_s64(_in))
#define nbt_endian_h2be_int(_in)   (nbt_endian_h2be_s32(_in))
#define nbt_endian_h2be_short(_in) (nbt_endian_h2be_s16(_in))
#define nbt_endian_h2be_byte(_in)  (_in)

#define nbt_endian_be2h_long(_in)  (nbt_endian_be2h_s64(_in))
#define nbt_endian_be2h_int(_in)   (nbt_endian_be2h_s32(_in))
#define nbt_endian_be2h_short(_in) (nbt_endian_be2h_s16(_in))
#define nbt_endian_be2h_byte(_in)  (_in)

#endif /* include guard */
