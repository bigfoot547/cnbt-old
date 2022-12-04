#ifndef LIBNBT_DEF_H_INCLUDED
#define LIBNBT_DEF_H_INCLUDED

#include <stdint.h>

#define NBT_FOREACH_INT_TYPE(_macro)                   \
_macro(int8_t,                      BYTE,       byte)  \
_macro(int16_t,                     SHORT,      short) \
_macro(int32_t,                     INT,        int)   \
_macro(int64_t,                     LONG,       long)

#define NBT_FOREACH_FP_TYPE(_macro)                    \
_macro(float,                       FLOAT,      float) \
_macro(double,                      DOUBLE,     double)

#define NBT_FOREACH_NUM_TYPE(_macro)                   \
_macro(int8_t,                      BYTE,       byte)  \
_macro(int16_t,                     SHORT,      short) \
_macro(int32_t,                     INT,        int)   \
_macro(int64_t,                     LONG,       long)  \
_macro(float,                       FLOAT,      float) \
_macro(double,                      DOUBLE,     double)

#define NBT_FOREACH_TYPE(_macro)                        \
_macro(int8_t,                  BYTE,       byte)       \
_macro(int16_t,                 SHORT,      short)      \
_macro(int32_t,                 INT,        int)        \
_macro(int64_t,                 LONG,       long)       \
_macro(float,                   FLOAT,      float)      \
_macro(double,                  DOUBLE,     double)     \
_macro(struct nbt_byte_array *, BYTE_ARRAY, byte_array) \
_macro(struct nbt_string *,     STRING,     string)     \
_macro(struct nbt_list *,       LIST,       list)       \
_macro(struct nbt_compound *,   COMPOUND,   compound)   \
_macro(struct nbt_int_array *,  INT_ARRAY,  int_array)  \
_macro(struct nbt_long_array *, LONG_ARRAY, long_array)

typedef uint8_t nbt_type;
typedef uint16_t nbt_strlen;

#define O(_ctype, _uname, _lname) \
typedef _ctype nbt_ ## _lname;
NBT_FOREACH_NUM_TYPE(O)
#undef O

struct nbt_byte_array {
    nbt_int len;
    nbt_byte *buf;
};

struct nbt_string {
    nbt_strlen len;
    char *buf;
};

struct nbt_list;
struct nbt_compound;

struct nbt_int_array {
    nbt_int len;
    nbt_int *buf;
};

struct nbt_long_array {
    nbt_int len;
    nbt_long *buf;
};

enum {
    NBT_TAG_END = 0,
    #define O(_ctype, _uname, _lname) \
        NBT_TAG_ ## _uname,
        NBT_FOREACH_TYPE(O)
    #undef O
};

typedef union {
#define O(_ctype, _uname, _lname) \
        _ctype tag_ ## _lname;

        NBT_FOREACH_TYPE(O)
#undef O
} nbt_value;

struct nbt_tag {
    nbt_type type;
    nbt_value value;
};

struct nbt_list_entry;

struct nbt_list {
    nbt_type type;
    nbt_int length;
    struct nbt_list_entry *first;
};

struct nbt_list_entry {
    nbt_value value;
    struct nbt_list_entry *next;
};

struct nbt_compound_entry;

struct nbt_compound {
    uint32_t size; /* extension */
    struct nbt_compound_entry *first;
};

struct nbt_compound_entry {
    nbt_strlen namelen;
    char *name;
    struct nbt_tag tag;
    struct nbt_compound_entry *next;
};

struct nbt_parsed {
    nbt_strlen namelen;
    char *name;

    struct nbt_compound *root;
};

#endif /* include guard */
