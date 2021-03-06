#include <stdio.h>
#include <string.h>
#include "WC_HashTable.h"

int main(void) {

    struct hash_table* new_table = hash_table_new();

    char* key_one = "Heil_Hydra";
    char* value_one = "COOHTMSTIP";

    char* key_two = "MSI";
    char* value_two = "Microsoft";

    char* key_three = "Purple";
    char* value_three = "oompaloompa";

    char* key_four = "power";
    char* value_four = "Of One";

    hash_table_add(new_table, key_one, strlen(key_one) + 1,
                   value_one, strlen(value_one) + 1);
    hash_table_add(new_table, value_one, strlen(value_one) + 1,
                   key_one, strlen(key_one) + 1);
    hash_table_add(new_table, key_two, strlen(key_two) + 1,
                   value_two, strlen(value_two) + 1);
    hash_table_add(new_table, value_two, strlen(value_two) + 1,
                   key_two, strlen(key_two) + 1);
    hash_table_add(new_table, key_three, strlen(key_three) + 1,
                   value_three, strlen(value_three) + 1);
    hash_table_add(new_table, value_three, strlen(value_three) + 1,
                   key_three, strlen(key_three) + 1);
    hash_table_add(new_table, key_four, strlen(key_four) + 1,
                   value_four, strlen(value_four) + 1);
    hash_table_add(new_table, value_four, strlen(value_four) + 1,
                   key_four, strlen(key_four) + 1);

    struct hash_table_key_value test_value = hash_table_get(new_table, key_one, strlen(key_one) + 1);
    printf("Value of key %s: %s\n", key_one, (char*)test_value.value);

    //print out the size of the table (number of elements stored)
    printf("Size of the table: %I64d\n", hash_table_size(new_table));

    unsigned char remove_success = hash_table_remove(new_table, key_one, strlen(key_one) + 1);
    printf("Remove successful? %d.\n", remove_success);
    
    test_value = hash_table_get(new_table, key_one, strlen(key_one) + 1);
    printf("Value of key %s: %s\n", key_one, (char*)test_value.value);

    //print out the size of the table (number of elements stored)
    printf("Size of the table: %I64d\n", hash_table_size(new_table));

    hash_table_free(new_table);
    printf("Done.\n");
    return 0;
}
