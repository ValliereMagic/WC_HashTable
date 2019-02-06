#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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

    table_element_t* new_element = malloc(sizeof(table_element_t));

    //System out of memory.
    if (new_element == NULL) {
        
        fprintf(stderr, "Error. System out of memory.\n");

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

//Calculate the next size of the hash table,
//by first doubling the current size,
//then finding the next prime.
//Return 0 on failure.
size_t find_next_table_length(hash_table_t* h_table) {

    //Make sure that the passed table actually exists.
    if (h_table == NULL) {

        fprintf(stderr, "Error. passed hash table is null in find next length.\n");

        return 0;
    }

    //Retrieve the current length of the hash table passed.
    size_t current_table_length = h_table->table_length;

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

    //Make sure that the passed element to add is not null.
    if (element == NULL) {

        fprintf(stderr, "Error. element passed in is NULL.\n");

        return 0;
    }

    //Make sure that the hash table passed actually exists.
    if (h_table == NULL) {

        fprintf(stderr, "Error. passed h_table doesn't exist in add.\n");

        return 0;
    }

    //Pull key, value and key_length out of the passed element.
    void* key = element->key;

    size_t key_length = element->key_length;

    void* value = element->value;

    //make sure that table, key, and value exist.
    if (key == NULL || value == NULL || h_table == NULL) {

        fprintf(stderr, "Error. h_table, key, or value is NULL in add_element.\n");

        return 0;
    }

    //check if hash table is at capacity, and if it is,
    //double its size before adding the next element.
    if (h_table->elements_stored >= h_table->table_capacity) {

        //Double the size of the table, and add all the elements from the old table
        //to the new one.

        //Uses h_table
        {
            size_t next_table_length = find_next_table_length(h_table);

            //make sure that the hash table passed exists.
            if (next_table_length == 0) {
                
                fprintf(stderr, "Error. find next index is 0.\n");

                return 0;
            }

            table_element_t** prev_table = h_table->table;

            //make sure the previous table exists.
            if (prev_table == NULL) {
                
                fprintf(stderr, "Error. previous table on realloc is NULL.\n");

                return 0;
            }

            //copy out previous hash table
            size_t table_previous_length = h_table->table_length;

            table_element_t* temp_table[table_previous_length];

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
        }
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
        //printf("infinite loop?\n");
        //increment the table index by the offset required
        //for quadratic probing.
        table_index += offset;

        wrapped_index = table_index % table_length;

        printf("Wrapped index: %I64d\n", wrapped_index);

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

    return 0;
}

//returns the value stored at the key passed.
//will return null if there is nothing stored at the key passed.
void* hash_table_get(hash_table_t* h_table, void* key, size_t key_length) {

    return NULL;
}
