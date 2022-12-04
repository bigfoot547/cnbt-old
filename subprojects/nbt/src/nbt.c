#include "nbt.h"
#include "nbt_def.h"
#include "nbt_endian.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include <setjmp.h>

#include <zlib.h>

#define NBT_ERROR_BUF_SZ (512)
char nbt_error_buf[NBT_ERROR_BUF_SZ] = { '\0' };

const char *nbt_error(void) {
    return nbt_error_buf;
}

void nbt_copy_error(const char *buf) {
    strncpy(nbt_error_buf, buf, NBT_ERROR_BUF_SZ);
    nbt_error_buf[NBT_ERROR_BUF_SZ-1] = '\0';
}

void nbt_set_error(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vsnprintf(nbt_error_buf, NBT_ERROR_BUF_SZ, fmt, va);
    va_end(va);

    nbt_error_buf[NBT_ERROR_BUF_SZ-1] = '\0';
}

#define GZ_MAGIC_0 (0x1F)
#define GZ_MAGIC_1 (0x8B)

#if 0
int nbt_read(const unsigned char *data, size_t length, struct nbt_parsed *result) {

}
#endif

#define NBT_READ_EXCEPTION(_ex, _fmt, ...) \
do {                                       \
    nbt_set_error(_fmt, ## __VA_ARGS__);   \
    longjmp(_ex, 1);                       \
} while (0)

/* How to use NBT_HANDLE_EX:
 *
 * NBT_HANDLE_EX(newex) {
 *     ... run if NBT_TRY's contents cause an exception ...
 * } NBT_TRY(oldex) {
 *     ... code to run safely (pass newex to functions here) ...
 * } NBT_TRY_END
 */

#define NBT_HANDLE_EX(_newex)      \
{                                  \
    jmp_buf _newex;                \
    if (setjmp(_newex) != 0) { do

#define NBT_TRY(_oldex) while (0); longjmp(_oldex, 1); } do

#define NBT_TRY_END while (0); }

nbt_type nbt_read_type(gzFile file, jmp_buf ex);
nbt_strlen nbt_read_strlen(gzFile file, jmp_buf ex);
char *nbt_read_string(gzFile file, nbt_strlen *len, jmp_buf ex);

nbt_value nbt_read_value(gzFile file, nbt_type type, jmp_buf ex);

int nbt_read_file(FILE *file, struct nbt_parsed *result) {
    gzFile gzfp;

    int fd = dup(fileno(file));
    gzfp = gzdopen(fd, "rb");

    gzbuffer(gzfp, 32768); /* 32K buffer */

    result->namelen = 0;
    result->name = NULL;
    result->root = NULL;

    jmp_buf exjmp;
    if (setjmp(exjmp) != 0) {
        /* parsing exception */
        goto parse_error_cleanup;
    }

    nbt_type roottype = nbt_read_type(gzfp, exjmp);
    if (roottype != NBT_TAG_COMPOUND) {
        NBT_READ_EXCEPTION(exjmp, "Root tag is not TAG_COMPOUND (%#02hhx)", roottype);
    }

    result->name = nbt_read_string(gzfp, &result->namelen, exjmp);

    result->root = nbt_read_value(gzfp, roottype, exjmp).tag_compound;

    gzclose(gzfp);

    return 0;

parse_error_cleanup:
    gzclose(gzfp);

    free(result->name);
    nbt_free_compound(result->root);

    return -1;
}

nbt_type nbt_read_type(gzFile file, jmp_buf ex) {
    nbt_type ret;
    if (gzread(file, &ret, 1) < 1) NBT_READ_EXCEPTION(ex, "Failed to read NBT type: I/O error or EOF");
    return ret;
}

nbt_strlen nbt_read_strlen(gzFile file, jmp_buf ex) {
    uint16_t ret;
    if (gzread(file, &ret, 2) < 2) NBT_READ_EXCEPTION(ex, "Failed to read NBT u16: I/O error or EOF");
    return nbt_endian_be2h_u16(ret);
}

char *nbt_read_string(gzFile file, nbt_strlen *len, jmp_buf ex) {
    *len = nbt_read_strlen(file, ex);

    char *str = (char *)malloc(*len + 1);
    if (!str) NBT_READ_EXCEPTION(ex, "Failed to allocate space for NBT string: malloc() returned NULL");

    int nread;
    if ((nread = gzread(file, str, *len)) < *len) {
        free(str);
        NBT_READ_EXCEPTION(ex, "Partial read on NBT string: %d < %hu", nread, *len);
    }
    str[*len] = '\0';

    return str;
}

nbt_byte nbt_read_byte(gzFile file, jmp_buf ex);
nbt_short nbt_read_short(gzFile file, jmp_buf ex);
nbt_int nbt_read_int(gzFile file, jmp_buf ex);
nbt_long nbt_read_long(gzFile file, jmp_buf ex);

nbt_float nbt_read_float(gzFile file, jmp_buf ex);
nbt_double nbt_read_double(gzFile, jmp_buf ex);

struct nbt_byte_array *nbt_read_byte_array(gzFile file, jmp_buf ex);
struct nbt_string *nbt_read_tag_string(gzFile file, jmp_buf ex);
struct nbt_list *nbt_read_list(gzFile file, jmp_buf ex);
struct nbt_compound *nbt_read_compound(gzFile file, jmp_buf ex);
struct nbt_int_array *nbt_read_int_array(gzFile file, jmp_buf ex);
struct nbt_long_array *nbt_read_long_array(gzFile file, jmp_buf ex);

nbt_value nbt_read_value(gzFile file, nbt_type type, jmp_buf ex) {
    nbt_value ret;
    
    switch (type) {
        case NBT_TAG_BYTE:
            ret.tag_byte = nbt_read_byte(file, ex);
            break;
        case NBT_TAG_SHORT:
            ret.tag_short = nbt_read_short(file, ex);
            break;
        case NBT_TAG_INT:
            ret.tag_int = nbt_read_int(file, ex);
            break;
        case NBT_TAG_LONG:
            ret.tag_long = nbt_read_long(file, ex);
            break;
        case NBT_TAG_FLOAT:
            ret.tag_float = nbt_read_float(file, ex);
            break;
        case NBT_TAG_DOUBLE:
            ret.tag_double = nbt_read_double(file, ex);
            break;
        case NBT_TAG_BYTE_ARRAY:
            ret.tag_byte_array = nbt_read_byte_array(file, ex);
            break;
        case NBT_TAG_STRING:
            ret.tag_string = nbt_read_tag_string(file, ex);
            break;
        case NBT_TAG_LIST:
            ret.tag_list = nbt_read_list(file, ex);
            break;
        case NBT_TAG_COMPOUND:
            ret.tag_compound = nbt_read_compound(file, ex);
            break;
        case NBT_TAG_INT_ARRAY:
            ret.tag_int_array = nbt_read_int_array(file, ex);
            break;
        case NBT_TAG_LONG_ARRAY:
            ret.tag_long_array = nbt_read_long_array(file, ex);
            break;
    }

    return ret;
}

#define O(_ctype, _uname, _lname)                                                                       \
_ctype nbt_read_ ## _lname(gzFile file, jmp_buf ex) {                                                   \
    _ctype ret;                                                                                         \
    int nread;                                                                                          \
    if ((nread = gzread(file, &ret, sizeof(_ctype))) < sizeof(_ctype))                                  \
        NBT_READ_EXCEPTION(ex, "Partial read on NBT " #_lname ": %d < %d", nread, (int)sizeof(_ctype)); \
    return nbt_endian_be2h_ ## _lname(ret);                                                             \
}

NBT_FOREACH_INT_TYPE(O)
#undef O

#define O(_ctype, _uname, _lname)                                                                       \
_ctype nbt_read_ ## _lname(gzFile file, jmp_buf ex) {                                                   \
    _ctype ret;                                                                                         \
    int nread;                                                                                          \
    if ((nread = gzread(file, &ret, sizeof(_ctype))) < sizeof(_ctype))                                  \
        NBT_READ_EXCEPTION(ex, "Partial read on NBT " #_lname ": %d < %d", nread, (int)sizeof(_ctype)); \
    return ret;                                                                                         \
}

NBT_FOREACH_FP_TYPE(O)
#undef O

#define NBT_READ_ARRAY(_t) \
struct nbt_ ## _t ## _array *nbt_read_ ## _t ## _array(gzFile file, jmp_buf ex) {                                                     \
    struct nbt_ ## _t ## _array *ret = malloc(sizeof(struct nbt_ ## _t ## _array));                                                   \
    if (!ret)                                                                                                                         \
        NBT_READ_EXCEPTION(ex, "Unable to allocate memory for new NBT " #_t "_array");                                                \
                                                                                                                                      \
    memset(ret, 0, sizeof(struct nbt_ ## _t ## _array));                                                                              \
                                                                                                                                      \
    NBT_HANDLE_EX(newex) {                                                                                                            \
        nbt_free_ ## _t ## _array(ret);                                                                                               \
    } NBT_TRY(ex) {                                                                                                                   \
        ret->len = nbt_read_int(file, newex);                                                                                         \
                                                                                                                                      \
        if (ret->len < 0)                                                                                                             \
            NBT_READ_EXCEPTION(newex, "NBT " #_t " array has negative length: %d", ret->len);                                         \
        else if (ret->len == 0)                                                                                                       \
            ret->buf = NULL;                                                                                                          \
        else {                                                                                                                        \
            size_t readlen = ret->len * sizeof(nbt_ ## _t);                                                                           \
            if (readlen > UINT_MAX)                                                                                                   \
                NBT_READ_EXCEPTION(newex, "Unable to allocate %zu bytes for nbt_" #_t "_array: length does not fit in unsigned int"); \
            unsigned int nread;                                                                                                       \
            ret->buf = malloc(readlen);                                                                                               \
            if (!ret->buf)                                                                                                            \
                NBT_READ_EXCEPTION(newex, "Unable to allocate %zu bytes for nbt_" #_t "_array buffer", readlen);                      \
                                                                                                                                      \
            if ((nread = gzread(file, ret->buf, readlen)) < readlen)                                                                  \
                NBT_READ_EXCEPTION(newex, "Partial read on NBT " #_t "_array: %lu < %zu", nread, readlen);                            \
        }                                                                                                                             \
    } NBT_TRY_END                                                                                                                     \
    return ret;                                                                                                                       \
}

NBT_READ_ARRAY(byte)

#if 0
struct nbt_byte_array *nbt_read_byte_array(gzFile file, jmp_buf ex) {
    struct nbt_byte_array *ret = malloc(sizeof(struct nbt_byte_array));
    if (!ret)
        NBT_READ_EXCEPTION(ex, "Unable to allocate memory for new NBT byte_array");
    
    memset(ret, 0, sizeof(struct nbt_byte_array));

    NBT_HANDLE_EX(newex) {
        nbt_free_byte_array(ret);
    } NBT_TRY(ex) {
        ret->len = nbt_read_int(file, newex);

        if (ret->len < 0)
            NBT_READ_EXCEPTION(newex, "NBT byte array has negative length: %d", ret->len);
        else if (ret->len == 0)
            ret->buf = NULL;
        else {
            size_t readlen = ret->len * sizeof(nbt_byte);
            unsigned int nread;
            ret->buf = malloc(readlen);
            if (!ret->buf)
                NBT_READ_EXCEPTION(newex, "Unable to allocate %zu bytes for nbt_byte buffer", readlen);

            if ((nread = gzread(file, ret->buf, readlen)) < readlen)
                NBT_READ_EXCEPTION(newex, "Partial read on NBT byte_array: %lu < %zu", nread, readlen);
        }
    } NBT_TRY_END

    return ret;
}
#endif

struct nbt_string *nbt_read_tag_string(gzFile file, jmp_buf ex) {
    struct nbt_string *ret = malloc(sizeof(struct nbt_string));
    if (!ret)
        NBT_READ_EXCEPTION(ex, "Unable to allocate memory for new NBT string");
    
    memset(ret, 0, sizeof(struct nbt_string));
    
    NBT_HANDLE_EX(newex) {
        nbt_free_string(ret);
    } NBT_TRY(ex) {
        ret->buf = nbt_read_string(file, &ret->len, newex);
    } NBT_TRY_END

    return ret;
}

struct nbt_list *nbt_read_list(gzFile file, jmp_buf ex) {
    struct nbt_list *ret = malloc(sizeof(struct nbt_list));
    if (!ret)
        NBT_READ_EXCEPTION(ex, "Unable to allocate memory for new NBT list");

    memset(ret, 0, sizeof(struct nbt_list));
    
    NBT_HANDLE_EX(newex) {
        nbt_free_list(ret);
    } NBT_TRY(ex) {
        ret->type = nbt_read_type(file, newex);
        ret->length = nbt_read_int(file, newex);

        if (ret->length < 0)
            NBT_READ_EXCEPTION(newex, "NBT list has negative length: %d", ret->length);
        else if (ret->length == 0)
            ret->first = NULL;
        else {
            if (ret->type == NBT_TAG_END)
                NBT_READ_EXCEPTION(newex, "NBT list has %d (> 0) value(s) of type NBT_TAG_END", ret->length);

            struct nbt_list_entry **entry = &ret->first;
            for (nbt_int i = 0; i < ret->length; ++i) {
                *entry = malloc(sizeof(struct nbt_list_entry));
                if (!(*entry))
                    NBT_READ_EXCEPTION(newex, "Unable to allocate memory for NBT list entry");
                
                memset(*entry, 0, sizeof(struct nbt_list_entry));

                (*entry)->value = nbt_read_value(file, ret->type, newex);
                entry = &(*entry)->next;
            }
            *entry = NULL;
        }
    } NBT_TRY_END

    return ret;
}

struct nbt_compound *nbt_read_compound(gzFile file, jmp_buf ex) {
    struct nbt_compound *ret = malloc(sizeof(struct nbt_compound));
    if (!ret)
        NBT_READ_EXCEPTION(ex, "Unable to allocate memory for new NBT compound");
    
    memset(ret, 0, sizeof(struct nbt_compound));
    
    NBT_HANDLE_EX(newex) {
        nbt_free_compound(ret);
    } NBT_TRY(ex) {
        struct nbt_compound_entry **entry = &ret->first;
        nbt_type elemtype;
        ret->first = NULL;
        ret->size = 0;

        while (true) {
            *entry = malloc(sizeof(struct nbt_compound_entry));
            if (!(*entry))
                NBT_READ_EXCEPTION(newex, "Unable to allocate memory for NBT compound entry");

            memset(*entry, 0, sizeof(struct nbt_compound_entry));

            elemtype = nbt_read_byte(file, newex);
            if (elemtype == NBT_TAG_END) break;

            (*entry)->name = nbt_read_string(file, &(*entry)->namelen, newex);
            (*entry)->tag.type = elemtype;
            (*entry)->tag.value = nbt_read_value(file, elemtype, newex);
            entry = &(*entry)->next;
            ++ret->size;
        }
    } NBT_TRY_END;

    return ret;
}

NBT_READ_ARRAY(int)
NBT_READ_ARRAY(long)

#undef NBT_READ_ARRAY
