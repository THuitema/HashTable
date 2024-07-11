#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hash_table.h"

/*
check for memory leaks:
"leaks --atExit -- ./<file>"
*/

static entry DELETED_ENTRY = {NULL, NULL};

static entry* create_entry(const char* key, const char* value) {
    entry* e = malloc(sizeof(entry));

    e->key = malloc(strlen(key) + 1);
    e->value = malloc(strlen(value) + 1);
    strcpy(e->key, key);
    strcpy(e->value, value);

    return e;
}

static void delete_entry(entry* e) {
    free(e->key);
    free(e->value);
    free(e);
}

static void ht_delete(hash_table* ht) {
    for(int i = 0; i < ht->size; i++) {
        entry* curr = ht->entries[i];
        if(curr != NULL && curr != &DELETED_ENTRY) {
            delete_entry(curr);
        }
    }

    free(ht->entries);
    free(ht);
}

static hash_table* ht_create(int size) {
    hash_table* ht = malloc(sizeof(hash_table));

    ht->size = size;
    ht->count = 0;
    ht->entries = calloc(size, sizeof(entry*));

    return ht;
}

static void ht_resize(hash_table* ht) {
    hash_table* ht_new = ht_create(ht->size * 2);
    for(int i = 0; i < ht->size; i++) {
        entry* curr = ht->entries[i];
        if(curr != NULL && curr != &DELETED_ENTRY) {
            ht_add(ht_new, curr->key, curr->value);
        }
    }

    /* Swap attributes of new and old hashtables, then delete new_ht */
    int old_size = ht->size;
    ht->size = ht_new->size;
    ht_new->size = old_size;

    entry** old_entries = ht->entries;
    ht->entries = ht_new->entries;
    ht_new->entries = old_entries;

    ht_delete(ht_new);
}

static int hash(const char* key, int table_size) {
    long hash = 0;
    
    // loop through characters in key (ASCII values)
    for(int i = 0; i < strlen(key); i++) {
        hash += (int) pow(key[i], strlen(key) - i) * 149;
    }

    return (int) (hash % table_size);
}

void ht_add(hash_table* ht, const char* key, const char* value) {
    int index = hash(key, ht->size);
    entry* item = create_entry(key, value);
    entry* curr = ht->entries[index];

    /* Perform linear probing if there is a collision */
    while(curr != NULL && curr != &DELETED_ENTRY) {
        /* If key exists, update its value */
        if(!strcmp(key, curr->key)) {
            delete_entry(curr);
            ht->entries[index] = item;
            return;
        }

        index++;
        /* Loop around to first index if end is reached */
        if(index == ht->size) {
            index = 0;
        }
        curr = ht->entries[index];
    }
    
    ht->entries[index] = item;
    ht->count += 1;

    /* Check if table needs to be resized */
    if (ht->count / (double) ht->size >= LOAD_FACTOR) {
        ht_resize(ht);
    }
}

char* ht_get(hash_table* ht, const char* key) {
    int index = hash(key, ht->size);
    entry* item = ht->entries[index];

    /* Search for key using linear probing */
    while(item != NULL) {
        if(item != &DELETED_ENTRY) {
            if(!strcmp(key, item->key)) {
                return item->value;
            }
        }
        index++;
        item = ht->entries[index];
    }

    /* No key found, return NULL */
    return NULL;
}

int ht_remove(hash_table* ht, const char* key) {
    int index = hash(key, ht->size);
    entry* item = ht->entries[index];

    while(item != NULL) {
        if(item != &DELETED_ENTRY) {
            if(!strcmp(key, item->key)) {
                delete_entry(item);
                ht->entries[index] = &DELETED_ENTRY; /* We don't "delete" entries, instead we mark them as deleted */
                ht->count--;
                return 1; 
            }
        }
        index++;
        item = ht->entries[index];
    }

    return 0;
}


int main() {
    hash_table* ht = ht_create(10);
    ht_add(ht, "T", "T");
    ht_add(ht, "V", "V");
    ht_add(ht, "I", "I");
    ht_add(ht, "G", "G");
    ht_add(ht, "A", "A");
    ht_add(ht, "B", "B");
    ht_remove(ht, "A");
    ht_remove(ht, "A");
    printf("Count: %d Size: %d\n", ht->count, ht->size);
    ht_add(ht, "C", "C");
    printf("Count: %d Size: %d\n", ht->count, ht->size);
    ht_add(ht, "D", "D");
    printf("Count: %d Size: %d\n", ht->count, ht->size);
    printf("Find A (should be null): %s\n", ht_get(ht, "A"));


    ht_delete(ht);
    return 0;
}
