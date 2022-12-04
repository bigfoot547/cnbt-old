#ifndef LIBNBT_NBT_H_INCLUDED
#define LIBNBT_NBT_H_INCLUDED

#include <stdint.h>
#include <stdio.h> /* for FILE */
#include <stdbool.h>

#include "nbt_def.h"

void nbt_free_value(nbt_type type, nbt_value value);

void nbt_free_byte_array(struct nbt_byte_array *array);
void nbt_free_string(struct nbt_string *str);
void nbt_free_list(struct nbt_list *list);
void nbt_free_compound(struct nbt_compound *compound);
void nbt_free_int_array(struct nbt_int_array *array);
void nbt_free_long_array(struct nbt_long_array *array);

void nbt_free_tag(struct nbt_tag *tag);

const char *nbt_error(void);

int nbt_read(const unsigned char *data, size_t length, struct nbt_parsed *result);

int nbt_read_file(FILE *file, struct nbt_parsed *result);

/* TODO: write */

#endif /* include guard */
