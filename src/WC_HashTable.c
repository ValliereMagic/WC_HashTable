#include <stdlib.h>
#include <stdio.h>

typedef struct table_element {

    //value stored, accessable by key.
    void* value;

    //number of bytes the value takes up.
    size_t value_length;

    //key the value is stored under.
    void* key;

    //number of bytes the key takes up.
    size_t key_length;

} table_element_t;

typedef struct hash_table {

    //takes up deleted spaces in
    //the hash table.
    table_element_t deleted;

    //array of table elements
    table_element_t** table;

    //the current amount of elements
    //that the table can store at
    //it's current size.
    //This will be equal to 50% the length
    //of table. table's length must be
    //a prime number.
    size_t table_capacity;

    //actual length of the current table.
    //useful for freeing the values stored.
    size_t table_length;

    //the number of elements stored in the
    //hash table. Must be less than or
    //equal to the table_capacity.
    size_t elements_stored;

} hash_table_t;

/* Private HashTable functions*/

//djb2 hash function
size_t hash(unsigned char *str, size_t str_len) {

    size_t hash = 5381;

    for (int i = 0; i < str_len; i++) {

        hash = ((hash << 5) + hash) + str[i];
    }

    return hash;
}

/* Public HashTable functions */

//Create a new hash table. Returns a
//pointer to a new hash table on success.
//Returns NULL on failure.
hash_table_t* hash_table_new(void) {

    const size_t initial_table_size = 13;

    hash_table_t* new_hash_table = malloc(sizeof(hash_table_t));

    if (new_hash_table == NULL) {

        fprintf(stderr, "Error. System out of memory.\n");

        return NULL;
    }

    table_element_t** new_table = malloc(sizeof(table_element_t*) * initial_table_size);

    if (new_table == NULL) {

        free(new_hash_table);

        fprintf(stderr, "Error. System out of memory.\n");

        return NULL;
    }

    //initialize all the table elements of the hash table to NULL;
    for (size_t i = 0; i < initial_table_size; i++) {

        new_table[i] = NULL;
    }

    //initialize the newly allocated table to sane default values.

    new_hash_table->table = new_table;

    new_hash_table->table_capacity = initial_table_size / 2;

    new_hash_table->table_length = initial_table_size;

    new_hash_table->elements_stored = 0;

    return new_hash_table;
}

//free a passed hash_table from memory.
void hash_table_free(hash_table_t* h_table) {

    if (h_table == NULL) {

        fprintf(stderr, "Error. Attempting to free a NULL hash table.\n");
        
        return;
    }

    //pull the pointer to the start of the table out of h_table
    table_element_t** table = h_table->table;

    if (table == NULL) {

        free(h_table);

        fprintf(stderr, "Error. Hashtable is corrupt.");

        return;
    }

    //pull the memory address of the deleted dummy element
    //out of h_table.
    table_element_t* deleted = &h_table->deleted;

    //go through each element in the hash table, and
    //free every used element (not deleted or NULL)
    for (size_t i = 0; i < h_table->table_length; i++) {

        table_element_t* current = table[i];

        if (current != deleted && current != NULL) {
            
            free(current->value);

            free(current->key);

            free(current);

            table[i] = deleted;
        }
    }

    //free the table stored in the hash table
    //now that all the allocated elements are freed.
    free(table);

    //free the hash table itself, now that all of
    //it's dynamically allocated internals are freed.
    free(h_table);
}

//Add the passed value to the hash table accessable by the
//key passed.
//values and keys will be copied into memory managed by the
//hash table.
//function will return 1 on success, 0 on failure.
unsigned char hash_table_add(hash_table_t* h_table, void* key, size_t key_length,
                             void* value, size_t value_length) {

    return 0;
}

//removes the value stored at the key passed in the hash table
//returns 1 on success, 0 on failure.
unsigned char hash_table_remove(hash_table_t* h_table, void* key, size_t key_length) {

    return 0;
}

//returns the value stored at the key passed.
//will return null if there is nothing stored at the key passed.
void* hash_table_get(hash_table_t* h_table, void* key, size_t key_length) {

    return NULL;
}
