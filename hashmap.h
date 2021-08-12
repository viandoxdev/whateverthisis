#ifndef __HASHMAP_H
#define __HASHMAP_H
#include <stddef.h>
#include "genvec.h"

#define HASHMAP_DEFAULT_BUCKETS_COUNT 64
// max load factor before buckets resize
#define HASHMAP_LOAD_FACTOR_TRESHOLD 0.8

struct HashMapEntry {
	bool has_next;
	GenIndex next;
	size_t hash;
	bool occupied;
};

struct HashMap {
	size_t (*hash)(const void* data, size_t size);
	GenVec overflow;
	size_t buckets_count;
	void* buckets;
	size_t data_size;
	size_t members;
};

typedef struct HashMapEntry HashMapEntry;
typedef struct HashMap HashMap;

size_t  default_hash(const void* data, size_t size);
HashMap hashmap_new(size_t data_size);
float   hashmap_get_load_factor(HashMap* map);
void    hashmap_set_hash(HashMap* map, size_t hash, void* data);
void    hashmap_set(HashMap* map, void* key, size_t key_size, void* data);
bool    hashmap_has_hash(HashMap* map, size_t hash);
bool    hashmap_has(HashMap* map, void* key, size_t key_size);
void*   hashmap_get_hash(HashMap* map, size_t hash);
void*   hashmap_get(HashMap* map, void* key, size_t key_size);
void    hashmap_remove_hash(HashMap* map, size_t hash);
void    hashmap_remove(HashMap* map, void* key, size_t key_size);
void    hashmap_set_str(HashMap* map, char* key, void* data);
void    hashmap_remove_str(HashMap* map, char* key);
bool    hashmap_has_str(HashMap* map, char* key);
void*   hashmap_get_str(HashMap* map, char* key);
void    hashmap_resize(HashMap* map, size_t new_bucket_count);
void    hashmap_free(HashMap map);
void    hashmap_debug(HashMap map, char* format);
Vector  hashmap_values(HashMap* map);

#endif
