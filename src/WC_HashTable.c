#include <stdlib.h>

typedef struct hash_table {

    //array of void pointers
    //that point to each element stored
    //in the table
    void* table;

    //the current amount of elements
    //that the table can store at
    //it's current size.
    //This will be equal to 50% the length
    //of table. table's length must be
    //a prime number.
    size_t table_capacity;

    //the number of elements stored in the
    //hash table. Must be less than or
    //equal to the table_capacity.
    size_t elements_stored;

} hash_table_t;

/* Private HashTable functions*/

/* Public HashTable functions */

//Create a new hash table. Returns a
//pointer to a new hash table on success.
//Returns NULL on failure.
hash_table_t* hash_table_new(void) {

    return NULL;
}

//free a passed hash_table from memory.
void hash_table_free(hash_table_t* table) {

    return;
}

//Add the passed value to the hash table accessable by the
//key passed.
//values and keys will be copied into memory managed by the
//has table.
//function will return 1 on success, 0 on failure.
unsigned char hash_table_add(void* key, size_t key_length,
                             void* value, size_t value_length) {

    return 0;
}

//removes the value stored at the key passed in the hash table
//returns 1 on success, 0 on failure.
unsigned char hash_table_remove(void* key, size_t key_length) {

    return 0;
}

//returns the value stored at the key passed.
//will return null if there is nothing stored at the key passed.
void* hash_table_get(void* key, size_t key_length) {

    return NULL;
}
