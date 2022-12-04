#include "nbt.h"
#include "nbt_def.h"

#include <stdio.h>
#include <stdlib.h>

void print_tag(nbt_type type, nbt_value value) {
    switch (type) {
        case NBT_TAG_BYTE:
            printf("TAG_BYTE\n");
            break;
        case NBT_TAG_SHORT:
            printf("TAG_SHORT\n");
            break;
        case NBT_TAG_INT:
            printf("TAG_INT\n");
            break;
        case NBT_TAG_LONG:
            printf("TAG_LONG\n");
            break;
        case NBT_TAG_FLOAT:
            printf("TAG_FLOAT\n");
            break;
        case NBT_TAG_DOUBLE:
            printf("TAG_DOUBLE\n");
            break;
        case NBT_TAG_BYTE_ARRAY:
            printf("TAG_BYTE_ARRAY\n");
            break;
        case NBT_TAG_STRING:
            printf("TAG_STRING\n");
            break;
        case NBT_TAG_LIST:
            printf("TAG_LIST (%02hhx %d)\n", value.tag_list->type, value.tag_list->length);
            break;
        case NBT_TAG_COMPOUND:
            printf("TAG_COMPOUND\n");
            for (struct nbt_compound_entry *cur = value.tag_compound->first; cur; cur = cur->next) {
                printf("%s : ", cur->name);
                print_tag(cur->tag.type, cur->tag.value);
            }
            printf("TAG_END\n");
            break;
        case NBT_TAG_INT_ARRAY:
            printf("TAG_INT_ARRAY\n");
            break;
        case NBT_TAG_LONG_ARRAY:
            printf("TAG_LONG_ARRAY\n");
            break;
    }
}

int main(int argc, char **argv) {
    FILE *file = fopen(argv[1], "rb");
    struct nbt_parsed nbt;
    int ret = nbt_read_file(file, &nbt);
    fclose(file);

    printf("%d %s\n", ret, nbt_error());
    if (ret < 0) return 0;

    nbt_value val = {.tag_compound = nbt.root};
    print_tag(NBT_TAG_COMPOUND, val);

    free(nbt.name);
    nbt_free_compound(nbt.root);

    return 0;
}
