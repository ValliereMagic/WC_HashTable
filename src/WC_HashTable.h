#ifndef WC_HASH_TABLE_H
    #define WC_HASH_TABLE_H
    //Struct for returning values from the get function
    //memory inside the returned value must not be modified.
    //If data returned from get needs to be manipulated,
    //it must be copied into other memory.
    struct hash_table_value {
        //Value returned
        void* value;
        //Number of bytes that value takes up
        size_t value_length;
    };
    //hash table type
    struct hash_table;
    //Create a new hash table. Returns a
    //pointer to a new hash table on success.
    //Returns NULL on failure.
    struct hash_table* hash_table_new(void);
    //free a passed hash_table from memory.
    void hash_table_free(struct hash_table* h_table);
    //Add the passed value to the hash table accessable by the
    //key passed.
    //values and keys will be copied into memory managed by the
    //has table.
    //function will return 1 on success, 0 on failure.
    unsigned char hash_table_add(struct hash_table* h_table, void* key, size_t key_length,
                                 void* value, size_t value_length);
    //removes the value stored at the key passed in the hash table
    //returns 1 on success, 0 on failure.
    unsigned char hash_table_remove(struct hash_table* h_table, void* key, size_t key_length);
    //returns the value stored at the key passed.
    //will return null if there is nothing stored at the key passed.
    struct hash_table_value hash_table_get(struct hash_table* h_table, void* key, size_t key_length);
    //returns the number of elements stored in the hash table
    size_t hash_table_size(struct hash_table* h_table);
#endif
