#include "anyhashmap.h"
#include "hashmap.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

AnyHashMap anyhashmap_new() {
	AnyHashMap res = {};
	res.map = hashmap_new(sizeof(void*));
	// default value here isn't really great
	// but this being what it is we can't really
	// make any estimate.
	res.allocated = 64;
	res.data = malloc(res.allocated);
	res.last = res.data;
	return res;
}

void anyhashmap_ensure_allocated(AnyHashMap* amap, size_t size) {
	if(amap->allocated < size) {
		amap->data = realloc(amap->data, (amap->allocated * 2 > size ? (amap->allocated *= 2) : (amap->allocated = size)));
	}
}

static void* __put_data(AnyHashMap* amap, void* data, size_t size) {
	anyhashmap_ensure_allocated(amap, size + sizeof(size_t));
	*(size_t*)amap->last = size;
	amap->last += sizeof(size_t);
	void* res = amap->last;
	amap->last += size;
	memcpy(res, data, size);
	return res;
}

void anyhashmap_set_hash(AnyHashMap* amap, size_t hash, void* data, size_t size) {
	void* ptr = __put_data(amap, data, size);
	hashmap_set_hash(&amap->map, hash, &ptr);
}
void anyhashmap_set(AnyHashMap* amap, void* key, size_t key_size, void* data, size_t size) {
	void* ptr = __put_data(amap, data, size);
	hashmap_set(&amap->map, key, key_size, &ptr);
}
void anyhashmap_set_str(AnyHashMap* amap, char* key, void* data, size_t size) {
	void* ptr = __put_data(amap, data, size);
	hashmap_set_str(&amap->map, key, &ptr);
}
void anyhashmap_remove_hash(AnyHashMap* amap, size_t hash) {
	hashmap_remove_hash(&amap->map, hash);
}
void anyhashmap_remove(AnyHashMap* amap, void* key, size_t key_size) {
	hashmap_remove(&amap->map, key, key_size);
}
void anyhashmap_remove_str(AnyHashMap* amap, char* key) {
	hashmap_remove_str(&amap->map, key);
}

void hashmap_optimize(AnyHashMap* amap) {
	Vector values = hashmap_values(&amap->map);
	size_t total = 0;
	for(int i = 0; i < values.used; i++) {
		total += *((*(size_t**)vector_get_ptr(&values, i)) - 1);
	}
	void* new_data = malloc(total);
	void* head = new_data;
	for(int i = 0; i < values.used; i++) {
		size_t* size = (*(size_t**)vector_get_ptr(&values, i)) - 1;
		memcpy(new_data, size, sizeof(size_t) + *size);
	}
	free(amap->data);
	amap->allocated = total;
	amap->data = new_data;
	vector_free(&values);
}
