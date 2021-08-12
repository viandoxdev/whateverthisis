#ifndef __ANYHASHMAP_H
#define __ANYHASHMAP_H
#include "hashmap.h"

struct AnyHashMap {
	HashMap map;
	void* data;
	size_t allocated;
	void* last;
};
typedef struct AnyHashMap AnyHashMap;

#endif
