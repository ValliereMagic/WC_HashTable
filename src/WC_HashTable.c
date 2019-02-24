#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "WC_HashTable.h"

typedef struct table_element {
    //key the value is stored under.
    void* key;
    //number of bytes the key takes up.
    size_t key_length;
    //value stored, accessable by key.
    void* value;
    //number of bytes the value takes up.
    size_t value_length;
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
    //Initial hash value
    size_t hash = 5381;

    for (size_t i = 0; i < str_len; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash;
}

//allocates a new hash_table element.
//Returns a allocated table element on success.
//Returns NULL on failure.
table_element_t* allocate_element(void* value, size_t value_length,
                                  void* key, size_t key_length) {
    //make sure that key and value exist.
    if (value == NULL || key == NULL) {
        fprintf(stderr, "Error. value or key passed to allocate element is NULL.\n");
        return NULL;
    }
    //Get the amount of memory required to store a table_element_t from the heap.
    table_element_t* new_element = malloc(sizeof(table_element_t));
    //System out of memory.
    if (new_element == NULL) {
        fprintf(stderr, "Error. System out of memory.\n");
        return NULL;
    }
    //allocate key, then value
    //and copy the passed values in
    void* key_value_pair[2] = {key, value};
    void* new_key_value_pair[2];
    size_t* key_value_length_pair[2] = {&key_length, &value_length};

    for (unsigned char i = 0; i < 2; i++) {
        new_key_value_pair[i] = malloc(*key_value_length_pair[i]);
        //System out of memory check.
        if (new_key_value_pair[i] == NULL) {
            fprintf(stderr, "Error. Failed to allocate adequate memory for an element.\n");
            
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

//Returns 1 on equal, 0 otherwise.
unsigned char is_equal(void* obj_one, size_t obj_one_length,
                       void* obj_two, size_t obj_two_length) {
    //Make sure that passed parameters exist
    if (obj_one == NULL || obj_two == NULL) {
        fprintf(stderr, "Error. Either obj_one or obj_two "
                        "is NULL in is_equal.\n");
        return 0;
    }
    //Objects of different lengths
    //cannot be equal.
    if (obj_one_length != obj_two_length) {
        return 0;
    }
    //check each byte of both objects, and check whether
    //they are equal.
    for (size_t i = 0; i < obj_one_length; i++) {
        
        if (*(unsigned char*)obj_one != *(unsigned char*)obj_two) {
            return 0;
        }
    }
    //The elements must be equal then
    return 1;
}

//Return the element that the key maps to,
//and the index it is at in the passed size_t element_index pointer
//
//On failure, the returned element will be NULL, and the element_index
//will be set to 0
table_element_t* get_element(hash_table_t* h_table, void* key, size_t key_length,
                             size_t* element_index) {
    //Make sure that the parameters passed exist.
    if (h_table == NULL || key == NULL || element_index == NULL) {
        fprintf(stderr, "Error. either key, h_table, or element_index "
                        "passed to get_element is NULL.\n");
        return NULL;
    }
    //Pull out relevant properties of hash table
    size_t table_length = h_table->table_length;
    table_element_t** table_elements = h_table->table;
    table_element_t* deleted = &h_table->deleted;
    //Make sure that pointer elements retrieved
    //exist.
    if (table_elements == NULL || deleted == NULL) {
        fprintf(stderr, "Error. table element array or deleted is NULL in "
                        "get_element when retrieving from h_table struct.\n");
        *element_index = 0;
        return NULL;
    }
    //offset for quadratic probing
    size_t offset = 1;
    //Find the index where the element should be stored.
    size_t search_start_index = hash(key, key_length) % table_length;
    //Get the element that should be element to return
    //(without considering collisions)
    table_element_t* current = table_elements[search_start_index];
    //There isn't an element stored at the key passed.
    if (current == NULL) {
        *element_index = 0;
        return NULL;
    }
    //Look for current until NULL is reached. 
    while (current != NULL) {
        //Don't try to check against stepping
        //stones.
        if (current != deleted) {
            void* current_key = current->key;
            size_t current_key_length = current->key_length;
            //Make sure that the
            //key pulled out of current
            //actually exists.
            if (current_key == NULL) {
                fprintf(stderr, "Error. A key retrieved from an element "
                                "was NULL in get_element.\n");
                *element_index = 0;
                return NULL;
            }
            //Found the element.
            if (is_equal(current_key, current_key_length, key, key_length)) {
                *element_index = search_start_index;
                return current;
            }
        }
        //Jump to the next position for quadratic probing.
        search_start_index = (search_start_index + offset) % table_length;
        //Set current to the element stored there.
        current = table_elements[search_start_index];
        //Increment the offset for the next time around.
        offset += 2;
    }
    //Didn't find the element
    *element_index = 0;
    return NULL;
}

//Calculate the next size of the hash table,
//by first doubling the current size,
//then finding the next prime.
//Return 0 on failure.
size_t find_next_table_length(size_t current_table_length) {
    //Calculate the new resized table length by doubling
    //the current length.
    size_t new_table_length = current_table_length * 2;
    //Truth value for whether the new table length is prime.
    unsigned char length_is_prime;
    //Increment new_table_length until it is prime
    do {
        //check whether new_table_length is prime.
        length_is_prime = 1;
        size_t loop_delim = sqrt(new_table_length);
        
        for (size_t i = 2; i < loop_delim; i++) {
            
            if (new_table_length % i == 0) {
                length_is_prime = 0;
                new_table_length += 1;
            }
        }
    } while (length_is_prime == 0);

    return new_table_length;
}

//Add the passed value 
unsigned char hash_table_add_element(hash_table_t* h_table, table_element_t* element) {
    //Make sure that the hash table passed actually exists.
    if (h_table == NULL) {
        fprintf(stderr, "Error. passed h_table doesn't exist in add.\n");
        return 0;
    }
    //Make sure that the passed element to add is not null.
    if (element == NULL) {
        fprintf(stderr, "Error. element passed in is NULL.\n");
        return 0;
    }
    //check if hash table is at capacity, and if it is,
    //double its size before adding the next element.
    if (h_table->elements_stored >= h_table->table_capacity) {
        //Double the size of the table, and add all the elements from the old table
        //to the new one.
        //copy out previous hash table
        size_t table_previous_length = h_table->table_length;
        size_t next_table_length = find_next_table_length(table_previous_length);
        //Pull out the current table from h_table
        //to be resized.
        table_element_t** prev_table = h_table->table;
        //make sure the previous table exists.
        if (prev_table == NULL) {
            fprintf(stderr, "Error. previous table on realloc is NULL.\n");
            return 0;
        }
        //This table holds the values currently stored in
        //the table, and are re-hashed and re-added
        //when the table is expanded to its new size.
        table_element_t** temp_table = malloc(sizeof(table_element_t*) * table_previous_length);
        //Copy all the values from the previous table
        //into the temporary table.
        memcpy(temp_table, prev_table, sizeof(table_element_t*) * table_previous_length);
        //reallocate the table to the new size.
        table_element_t** new_table = h_table->table = realloc(prev_table, sizeof(table_element_t*) * next_table_length);
        //Make sure the reallocated table still exists.
        if (new_table == NULL) {
            fprintf(stderr, "Error. hash table is NULL after reallocating in add element.\n");
            return 0;
        }
        //Initialize all elements in the new table to NULL.
        for (size_t i = 0; i < next_table_length; i++) {
            new_table[i] = NULL;
        }
        //Update the lengths in the h_table structure.
        h_table->table_capacity = next_table_length / 2;
        h_table->table_length = next_table_length;
        h_table->elements_stored = 0;
        //Add back all of the elements stored away in
        //temp_table to the newly resized table.
        table_element_t* deleted = &h_table->deleted;
        
        for (size_t i = 0; i < table_previous_length; i++) {
            table_element_t* current = temp_table[i];
            
            if (current != deleted && current != NULL) {
                hash_table_add_element(h_table, current);
            }
        }
        //Free the allocated temporary table
        free(temp_table);
    }
    //Pull key and key_length out of the passed element.
    void* key = element->key;
    size_t key_length = element->key_length;
    //make sure that key exists.
    if (key == NULL) {
        fprintf(stderr, "Error. key is NULL in add_element.\n");
        return 0;
    }
    //Pulls the current table length out of the hash table.
    size_t table_length = h_table->table_length;
    //Find the index at which to store the new element
    //by hashing it's key, and fitting it within the
    //bounds of the table.
    size_t table_index = hash(key, key_length) % table_length;
    //Stores the new wrapped index, that is updated
    //while resolving collisions.
    size_t wrapped_index = table_index;
    //retrieve the table from the h_table
    table_element_t** table = h_table->table;
    //retrieve deleted pointer from h_table
    table_element_t* deleted = &h_table->deleted;
    //Quadratically probe until there is no collision
    table_element_t* current_element = table[table_index];
    size_t offset = 1;
    //Will only enter loop on a collision.
    while (current_element != NULL && current_element != deleted) {
        //increment the table index by the offset required
        //for quadratic probing.
        table_index += offset;
        wrapped_index = table_index % table_length;
        current_element = table[wrapped_index];
        //increment the offset again for quadratic probing.
        offset += 2;
    }
    //add the passed element into the table.
    table[wrapped_index] = element;
    //Added an element to the table, therefore increment
    //number of elements stored in table.
    h_table->elements_stored++;
    return 0;
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
    //Make sure that the passed hash table actually exists.
    if (h_table == NULL) {
        fprintf(stderr, "Error. Attempting to free a NULL hash table.\n");
        return;
    }
    //pull the pointer to the start of the table out of h_table
    table_element_t** table = h_table->table;

    if (table == NULL) {
        free(h_table);
        fprintf(stderr, "Error. Hashtable is corrupt.\n");
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
    //create a new element containing the passed values.
    table_element_t* new_element = allocate_element(value, value_length,
                                                    key, key_length);
    //Use the private function to add the newly allocated element
    //into the hash table.
    return hash_table_add_element(h_table, new_element);
}

//removes the value stored at the key passed in the hash table
//returns 1 on success, 0 on failure.
unsigned char hash_table_remove(hash_table_t* h_table, void* key, size_t key_length) {
    //Make sure that parameters passed exist.
    if (h_table == NULL || key == NULL) {
        fprintf(stderr, "Error. Either NULL h_table or NULL key passed to "
                        "hash_table_remove.\n");
        return 0;
    }
    //Get the element at the key passed.
    size_t element_index;
    table_element_t* element_to_remove = get_element(h_table, key, key_length, &element_index);
    //Check if the get_element found
    //the element at the key successfully.
    if (element_to_remove == NULL) {
        return 0;
    }
    //Set the element in h_table to deleted.
    h_table->table[element_index] = &h_table->deleted;
    //free the element
    free_element(element_to_remove);
    //Update the count of stored items
    h_table->elements_stored--;
    //element was removed successfully.
    return 1;
}

//returns the value stored at the key passed.
//will return NULL if there is nothing stored at the key passed.
hash_table_value_t hash_table_get(hash_table_t* h_table, void* key, size_t key_length) {
    //Initialize value_to_return
    hash_table_value_t value_to_return;
    value_to_return.value = NULL;
    value_to_return.value_length = 0;
    //Make sure that the parameters passed exist
    if (h_table == NULL || key == NULL) {
        fprintf(stderr, "Error. either NULL key or table passed to hash_table_get.\n");
        return value_to_return;
    }
    //element index for passing to get_element
    size_t element_index;
    table_element_t* element = get_element(h_table, key, key_length, &element_index);
    //If the element wasn't found.
    if (element == NULL) {
        return value_to_return;
    }
    //Set the values in value_to_return to
    //reflect the values in the element.
    value_to_return.value = element->value;
    value_to_return.value_length = element->value_length;
    return value_to_return;
}
