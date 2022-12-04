#include "nbt.h"
#include "nbt_def.h"

#include <stdlib.h>

void nbt_free_value(nbt_type type, nbt_value value) {
    switch (type) {
        case NBT_TAG_BYTE_ARRAY:
            nbt_free_byte_array(value.tag_byte_array);
            break;
        case NBT_TAG_STRING:
            nbt_free_string(value.tag_string);
            break;
        case NBT_TAG_LIST:
            nbt_free_list(value.tag_list);
            break;
        case NBT_TAG_COMPOUND:
            nbt_free_compound(value.tag_compound);
            break;
        case NBT_TAG_INT_ARRAY:
            nbt_free_int_array(value.tag_int_array);
            break;
        case NBT_TAG_LONG_ARRAY:
            nbt_free_long_array(value.tag_long_array);
            break;
    }
}

void nbt_free_tag(struct nbt_tag *tag) {
    if (!tag) return;
    nbt_free_value(tag->type, tag->value);
    free(tag);
}

void nbt_free_byte_array(struct nbt_byte_array *array) {
    if (!array) return;
    free(array->buf);
    free(array);
}

void nbt_free_string(struct nbt_string *str) {
    if (!str) return;
    free(str->buf);
    free(str);
}

void nbt_free_list(struct nbt_list *list) {
    if (!list) return;

    for (struct nbt_list_entry *cur = list->first, *temp; cur; cur = temp) {
        temp = cur->next;
        nbt_free_value(list->type, cur->value);
        free(cur);
    }

    free(list);
}

void nbt_free_compound(struct nbt_compound *compound) {
    if (!compound) return;
    for (struct nbt_compound_entry *cur = compound->first, *temp; cur; cur = temp) {
        temp = cur->next;
        nbt_free_value(cur->tag.type, cur->tag.value);
        free(cur->name);
        free(cur);
    }

    free(compound);
}

void nbt_free_int_array(struct nbt_int_array *array) {
    if (!array) return;
    free(array->buf);
    free(array);
}

void nbt_free_long_array(struct nbt_long_array *array) {
    if (!array) return;
    free(array->buf);
    free(array);
}
