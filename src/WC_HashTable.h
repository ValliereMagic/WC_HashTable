#ifndef WC_HASH_TABLE_H

    #define WC_HASH_TABLE_H

    //hash table type
    typedef struct hash_table hash_table_t;

    //Create a new hash table. Returns a
    //pointer to a new hash table on success.
    //Returns NULL on failure.
    hash_table_t* hash_table_new(void);

    //free a passed hash_table from memory.
    void hash_table_free(hash_table_t* table);

    //Add the passed value to the hash table accessable by the
    //key passed.
    //values and keys will be copied into memory managed by the
    //has table.
    //function will return 1 on success, 0 on failure.
    unsigned char hash_table_add(void* key, size_t key_length,
                                void* value, size_t value_length);

    //removes the value stored at the key passed in the hash table
    //returns 1 on success, 0 on failure.
    unsigned char hash_table_remove(void* key, size_t key_length);

    //returns the value stored at the key passed.
    //will return null if there is nothing stored at the key passed.
    void* hash_table_get(void* key, size_t key_length);

#endif
