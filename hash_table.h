#define LOAD_FACTOR 0.7

typedef struct {
    char* key;
    char* value;
} entry;

typedef struct {
    int size;
    int count;
    entry** entries; // array of items
} hash_table;

/* Hash table functions */
void ht_add(hash_table* ht, const char* key, const char* value);
char* ht_get(hash_table* ht, const char* key);
int ht_remove(hash_table* ht, const char* key);
