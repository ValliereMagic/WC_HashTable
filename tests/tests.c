#include <stdio.h>
#include <string.h>
#include "WC_HashTable.h"

int main(void) {

    hash_table_t* new_table = hash_table_new();

    char* key = "Heil_Hydra";
    char* value = "COOHTMSTIP";

    hash_table_add(new_table, key, strlen(key) + 1,
                   value, strlen(value) + 1);

    hash_table_add(new_table, value, strlen(value) + 1,
                   key, strlen(key) + 1);

    hash_table_free(new_table);

    printf("Done.\n");

    return 0;
}
