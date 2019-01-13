#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

    for (size_t i = 0; i < str_len; i++) {

        hash = ((hash << 5) + hash) + str[i];
    }

    return hash;
}

//wrap the index value around to be within the current
//length of the table.
size_t wrap_index(size_t index, size_t table_length) {

    return ((table_length - 1) + index) % table_length;
}

//allocates a new hash_table element.
//Returns a allocated table element on success.
//Returns NULL on failure.
table_element_t* allocate_element(void* value, size_t value_length,
                                  void* key, size_t key_length) {

    //make sure that key and value exist.
    if (value == NULL || key == NULL) {

        return NULL;
    }

    table_element_t* new_element = malloc(sizeof(table_element_t));

    //System out of memory.
    if (new_element == NULL) {

        return NULL;
    }


    //key, then value.
    void* key_value_pair[2] = {key, value};

    void* new_key_value_pair[2];
    
    size_t* key_value_length_pair[2] = {&key_length, &value_length};

    for (unsigned char i = 0; i < 2; i++) {

        new_key_value_pair[i] = malloc(*key_value_length_pair[i]);

        //System out of memory check.
        if (new_key_value_pair[i] == NULL) {

            if (i == 1) {

                free(new_key_value_pair[0]);
            }

            return NULL;
        }

        //copy the bytes into their new container.
        memcpy(new_key_value_pair[i], key_value_pair[i], *key_value_length_pair[i]);

    }

    new_element->key = new_key_value_pair[0];

    new_element->key_length = key_length;

    new_element->value = new_key_value_pair[1];

    new_element->value_length = value_length;

    return new_element;
}

void free_element(table_element_t* element) {
    
    free(element->value);

    free(element->key);

    free(element);
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

            free_element(current);

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
    
    //make sure that table, key, and value exist.
    if (key == NULL || value == NULL || h_table == NULL) {

        return 0;
    }

    //check if hash table is at capacity

    if (h_table->elements_stored >= h_table->table_capacity) {

        //TODO RESIZE AND REHASH TABLE

        return 0;
    }

    size_t table_length = h_table->table_length;

    size_t table_index = hash(key, key_length) % table_length;

    size_t wrapped_index = table_index;

    //retrieve the table from the h_table
    table_element_t** table = h_table->table;

    //retrieve deleted pointer from h_table
    table_element_t* deleted = &h_table->deleted;

    //Quadratically probe until there is no collision
    table_element_t* current_element = table[table_index];

    size_t offset = 3;
    
    while (current_element != NULL && current_element != deleted) {

        //increment the table index by the offset required
        //for quadratic probing.
        table_index += offset;

        wrapped_index = wrap_index(table_index, table_length);

        current_element = table[wrapped_index];

        //increment the offset again for quadratic probing.
        offset += 2;
    }

    //create a new element containing the passed values.
    table_element_t* new_element = allocate_element(value, value_length,
                                                    key, key_length);

    //add the element into the table.
    table[wrapped_index] = new_element;

    //Added an element to the table, therefore increment
    //number of elements stored in table.
    h_table->elements_stored++;

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
