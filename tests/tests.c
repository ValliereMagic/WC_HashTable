#include <stdio.h>
#include "WC_HashTable.h"

int main(void) {

    hash_table_t* new_table = hash_table_new();

    hash_table_free(new_table);

    return 0;
}
