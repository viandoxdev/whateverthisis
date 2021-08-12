#include "hashmap.h"
#include "genvec.h"
#include "vector.h"
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// thanks https://stackoverflow.com/a/1640399/10289343
static unsigned long x=123456789, y=362436069, z=521288629;

unsigned long xorshf96(void) {          //period 2^96-1
unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

   t = x;
   x = y;
   y = z;
   z = t ^ x ^ y;

  return z;
}

unsigned long _hash_seed = 0;

uint64_t _get_block(const void* data, size_t size, int index) {
	size_t res = 0;
	size_t byte_offset = index * sizeof(uint64_t);
	memcpy(&res, data + byte_offset, size - byte_offset > sizeof(uint64_t) ? sizeof(uint64_t) : byte_offset);
	return res;
}

uint64_t _rotl64(uint64_t x, int8_t r) {
	return (x << r) | (x >> (64 - r));
}

uint64_t _fmix64(uint64_t k) {
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccd;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53;
	k ^= k >> 33;
	return k;
}

// lets call it what it is, a copy and paste of MurmurHash3 x64 128 bit xor folded onto 64 adapted to the arguments
size_t default_hash(const void* data, size_t size) {
	const int nblocks = size / (2 * sizeof(uint64_t));
	
	while(!_hash_seed) { _hash_seed = xorshf96(); }

	uint64_t h1 = _hash_seed;
	uint64_t h2 = h1;

	const uint64_t c1 = 0x87c37b91114253d5;
	const uint64_t c2 = 0x4cf5ad432745937f;
	
	for(int i = 0; i < nblocks; i++) {
		uint64_t k1 = _get_block(data, size, i * 2);
		uint64_t k2 = _get_block(data, size, i * 2 + 1);

		k1 *= c1; k1 = _rotl64(k1, 31); k1 *= c2; h1 ^= k1;
		h1 = _rotl64(h1, 27); h1 += h2; h1 = h1*5+0x52dce729;
		k2 *= c2; k2 = _rotl64(k2, 33); k2 *= c1; h2 ^= k2;
		h2 = _rotl64(h2, 31); h2 += h1; h2 = h2*5+0x38495ab5;
	}

	const uint8_t* tail = (const uint8_t*)(data + nblocks * 2 * sizeof(uint64_t));

	uint64_t k1 = 0;
	uint64_t k2 = 0;

	switch(size & 15) {
		case 15: k2 ^= ((uint64_t)tail[14]) << 48;
		case 14: k2 ^= ((uint64_t)tail[13]) << 40;
		case 13: k2 ^= ((uint64_t)tail[12]) << 32;
		case 12: k2 ^= ((uint64_t)tail[11]) << 24;
		case 11: k2 ^= ((uint64_t)tail[10]) << 16;
		case 10: k2 ^= ((uint64_t)tail[ 9]) <<  8;
		case  9: k2 ^= ((uint64_t)tail[ 8]) <<  0;
						 k2 *= c2; k2 = _rotl64(k2, 33); k2 *= c1; h2 ^= k2;
		case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
		case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
		case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
		case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
		case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
		case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
		case  2: k1 ^= ((uint64_t)tail[ 1]) <<  8;
		case  1: k1 ^= ((uint64_t)tail[ 0]) <<  0;
						 k1 *= c1; k1 = _rotl64(k1, 31); k1 *= c2; h1 ^= k1;
	}

	h1 ^= size; h2 ^= size;
	
	h1 += h2;
	h2 += h1;

	h1 = _fmix64(h1);
	h2 = _fmix64(h2);

	h1 += h2;
	h2 += h1;
	// fold into 64 bits
	h1 ^= h2;
	return h1;
}

HashMap hashmap_new(size_t data_size) {
	HashMap res = {};
	res.overflow = genvec_new(5, data_size + sizeof(HashMapEntry));
	res.data_size = data_size;
	res.hash = default_hash;
	res.buckets_count = HASHMAP_DEFAULT_BUCKETS_COUNT;
	res.buckets = calloc(res.buckets_count, (data_size + sizeof(HashMapEntry)));
	return res;
}
float hashmap_get_load_factor(HashMap* map) {
	return (float) map->members / map->buckets_count;
}
// this is here to avoid duplication between resize and set
void __hashmap_put_at_index(size_t index, size_t hash, void* buckets, GenVec* overflow, size_t data_size, void* data) {
	HashMapEntry* entry = buckets + index * (data_size + sizeof(HashMapEntry));
	if(!entry->occupied) { // simpler case
		entry->hash = hash;
		entry->occupied = true;
		entry->has_next = false;
		memcpy(((void*) entry) + sizeof(HashMapEntry), data, data_size);
	} else { // collision
		// go to end of chain
		HashMapEntry* last = entry;
		bool overwrite = false;
		while(1) {
			if(last->hash == hash) {
				overwrite = true;
				break;
			}
			if(last->has_next)
				last = genvec_get(overflow, last->next);
			else break;
		}
		if(overwrite) {
			memcpy(last + 1, data, data_size);
		} else {
			GenIndex new_slot = genvec_push(overflow, NULL);
			void* ptr = genvec_get(overflow, new_slot);
			HashMapEntry new_entry = {};
			new_entry.hash = hash;
			new_entry.occupied = true;
			new_entry.has_next = false;
			*((HashMapEntry*)ptr) = new_entry;
			memcpy(ptr + sizeof(HashMapEntry), data, data_size);
			last->has_next = true;
			last->next = new_slot;
		}
	}
}

size_t hashmap_get_hash_mask(size_t buckets_count) {
	// this turns 0b00...00100...00 (buckets_count)
	// then into  0b00...00011...11 (buckets_count - 1)
	return buckets_count - 1;
}

void hashmap_set_hash(HashMap* map, size_t hash, void* data) {
	size_t mask = hashmap_get_hash_mask(map->buckets_count);
	size_t index = hash & mask;
	__hashmap_put_at_index(index, hash, map->buckets, &map->overflow, map->data_size, data);
	map->members++;
	if(hashmap_get_load_factor(map) > HASHMAP_LOAD_FACTOR_TRESHOLD) {
		hashmap_resize(map, map->buckets_count * 2);
	}
}

void hashmap_set(HashMap* map, void* key, size_t key_size, void* data) {
	size_t hash = (*map->hash)(key, key_size);
	hashmap_set_hash(map, hash, data);
}

bool hashmap_has_hash(HashMap* map, size_t hash) {
	size_t mask = hashmap_get_hash_mask(map->buckets_count);
	size_t index = hash & mask;
	HashMapEntry* entry = map->buckets + index * map->overflow.data_size;
	if(!entry->occupied) return false;
	while(1) {
		if(entry->hash == hash) return true;
		if(entry->has_next)
			entry = genvec_get(&map->overflow, entry->next);
		else
			return false;
	}
}

bool hashmap_has(HashMap* map, void* key, size_t key_size) {
	size_t hash = (*map->hash)(key, key_size);
	return hashmap_has_hash(map, hash);
}

void* hashmap_get_hash(HashMap* map, size_t hash) {
	size_t mask = hashmap_get_hash_mask(map->buckets_count);
	size_t index = hash & mask;
	HashMapEntry* entry = map->buckets + index * map->overflow.data_size;
	if(!entry->occupied) return NULL;
	while(1) {
		if(entry->hash == hash) break;
		if(entry->has_next)
			entry = genvec_get(&map->overflow, entry->next);
		else
			return NULL;
	}
	return entry + 1;
}

void* hashmap_get(HashMap* map, void* key, size_t key_size) {
	size_t hash = (*map->hash)(key, key_size);
	return hashmap_get_hash(map, hash);
}

void hashmap_remove_hash(HashMap* map, size_t hash) {
	size_t mask = hashmap_get_hash_mask(map->buckets_count);
	size_t index = hash & mask;
	HashMapEntry* entry = map->buckets + index * map->overflow.data_size;
	HashMapEntry* prev = NULL;
	GenIndex gi = {}; // to remove the entry from overflow
	if(!entry->occupied) return;
	while(1) {
		if(entry->hash == hash) {
			break;
		}
		if(entry->has_next) {
			prev = entry;
			gi = entry->next;
			entry = genvec_get(&map->overflow, entry->next);
		} else {
			// missing
			return;
		}
	}
	if(prev != NULL) { // then entry is in a chain
		prev->has_next = entry->has_next;
		prev->next = entry->next;
		genvec_remove(&map->overflow, gi, NULL);
	} else if(entry->has_next) { // then entry is the head of a chain
		HashMapEntry* next = genvec_get(&map->overflow, entry->next);
		gi = entry->next;
		// overwrite entry with next
		*entry = *next;
		memcpy(entry + 1, next + 1, map->data_size);
		// remove next from overflow as it is now the head
		genvec_remove(&map->overflow, gi, NULL);
	} else {
		entry->occupied = false;
	}
	map->members--;
}

void hashmap_remove(HashMap* map, void* key, size_t key_size) {
	size_t hash = (*map->hash)(key, key_size);
	hashmap_remove_hash(map, hash);
}

void hashmap_set_str(HashMap* map, char* key, void* data) {
	hashmap_set(map, key, strlen(key) * sizeof(char), data);
}

void hashmap_remove_str(HashMap* map, char* key) {
	hashmap_remove(map, key, strlen(key) * sizeof(char));
}

bool hashmap_has_str(HashMap* map, char* key) {
	return hashmap_has(map, key, strlen(key) * sizeof(char));
}

void* hashmap_get_str(HashMap* map, char* key) {
	return hashmap_get(map, key, strlen(key) * sizeof(char));
}
void hashmap_resize(HashMap* map, size_t new_bucket_count) {
	void* new_buckets = calloc(map->data_size + sizeof(HashMapEntry), new_bucket_count);
	size_t new_mask = hashmap_get_hash_mask(new_bucket_count);
	GenVec new_overflow = genvec_new(10, map->overflow.data_size);
	for(size_t i = 0; i < map->buckets_count; i++) {
		HashMapEntry* entry = map->buckets + i * map->overflow.data_size;
		if(!entry->occupied) continue; // skip if empty
		while(1) {
			void* data = entry + 1; // +1 adds sizeof(HashMapEntry)
			size_t new_index = entry->hash & new_mask;
			__hashmap_put_at_index(new_index, entry->hash, new_buckets, &new_overflow, map->data_size, data);
			if(entry->has_next) {
				entry = genvec_get(&map->overflow, entry->next);
			} else {
				break;
			}
		}
	}
	free(map->buckets);
	genvec_free(map->overflow);
	map->buckets = new_buckets;
	map->overflow = new_overflow;
	map->buckets_count = new_bucket_count;
}

void hashmap_free(HashMap map) {
	free(map.buckets);
	genvec_free(map.overflow);
}

void _center(size_t length, char* format, uintmax_t value) {
	size_t l = snprintf(NULL, 0, format, value);
	char str[l + 1];
	int padleft = (length - l) / 2;
	int padright = length - l - padleft;
	snprintf(str, l + 1, format, value);
	printf("%*s%s%*s", padleft, "", str, padright, "");
}

Vector hashmap_values(HashMap* map) {
	Vector res = vector_new(map->members, map->data_size);
	for(int i = 0; i < map->buckets_count; i++) {
		HashMapEntry* entry = (HashMapEntry*)(map->buckets + i * map->overflow.data_size);
		if(entry->occupied) {
			vector_push(&res, entry + 1);
			while(entry->has_next) {
				entry = genvec_get(&map->overflow, entry->next);
				vector_push(&res, entry + 1);
			}
		}
	}
	return res;
}


// yes i know this really bad code, but its for debugging so its ok
void hashmap_debug(HashMap map, char* format) {
	printf("%s", "┌────────┬───────────────┬───────────┬─────────┐\n");
	printf("%s", "│  map   │ buckets count │ data size │ members │\n");
	printf("%s", "├────────┼───────────────┼───────────┼─────────┤\n");
	printf("%s", "│ values │");
	_center(15, "%lu", map.buckets_count);
	printf("%s", "│");
	_center(11, "%lu", map.data_size);
	printf("%s", "│");
	_center(9, "%lu", map.members);
	printf("%s", "│\n");
	printf("%s", "└────────┴───────────────┴───────────┴─────────┘\n");
	bool last_occ = true;
	bool last_n = false;
	for(size_t i = 0; i < map.buckets_count; i++) {
		HashMapEntry* e = map.buckets + map.overflow.data_size * i;
		bool locc = last_occ;
		last_occ = e->occupied;
		if(!locc && !last_occ) continue;
		if(locc && !last_occ) {
			if(i == 0) {
				printf("%s", "┌──────────────────────────────────────────┐\n");
			} else if(!last_n) {
				printf("%s", "\033[1A├────┴──────────────────┴──────────────────┤\n");
			}  else {
				printf("%s", "\033[1A┌────┴──────────────────┴──────────────────┤\n");
			}
			printf("%s", "│                   .....                  │\n");
			printf("%s", "├────┬──────────────────┬──────────────────┤\n");
			continue;
		}
		printf("%s", "│");
		_center(4, "%lu", i);
		printf("%s", "│");
		_center(18, "%016lx", e->hash);
		printf("%s", "│");
		_center(18, format, *((uintmax_t*)(e + 1)));
		printf("%s", "│\n");
		printf("%s────┼──────────────────┼──────────────────┤\n", e->has_next ? "└" : "├");
		bool hn = e->has_next;
		while(e->has_next) {
			e = genvec_get(&map.overflow, e->next);
			printf("     │");
			_center(18, "%016lx", e->hash);
			printf("│");
			_center(18, format, *((uintmax_t*)(e + 1)));
			printf("│\n");
			printf("     ├──────────────────┼──────────────────┤\n");
		}
		if(hn) {
			printf("\033[1A┌────┼──────────────────┼──────────────────┤\n");
		}
		last_n = hn;
	}
	if(last_occ) {
		printf("\033[1A└────┴──────────────────┴──────────────────┘\n");
	} else {
		printf("\033[1A└──────────────────────────────────────────┘\n");
	}
	printf("\n\n");
}
