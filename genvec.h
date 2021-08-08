#ifndef __GENVEC_H
#define __GENVEC_H

#include "vector.h"
#include <stdbool.h>
// memory layout:
//              Vector (single alloc)
//       ┌───────────────┬───────────────┬──╌╌╌╶╶╶
// index │       0       │       1       │       2 
//       ├─────┬─────────┼─────┬─────────┼──╌╌┬╶╶╶
//  data │     │         │     │         │    │   
//       └─────┴─────────┴─────┴─────────┴──╌╌┴╶╶╶
//    info ─┘     data
//  (size_t)   (data_size)
//
//       info: (size_t)
//
//          0b1000000000000000000000000000000000000000000000000000000000000000
// occupied   │└───────────────────────────┬─────────────────────────────────┘
// bit:      ─┘                            │
// 1: free        the rest is used to store, either, the generation, or the
// 0: occupied   the next_free, depending on occupied bit, 1: next_free, 0: gen

// if the GENOCCUPIED_BIT is 1, the entry in EMPTY
// if its 0, the entry is occupied.
#define GENVEC_GENOCCUPIED_BIT (((size_t)1) << 63)

struct GenVec {
	size_t generation;
	size_t data_size; 
	size_t last_free;
	Vector vec;
};

// not sure how to document that but, about last_free and next_free.
// next_free and last_free are fields on entries (indirectly*) and GenVec
// that indicates the index of the last free entry, they act kind of like
// a linked list:
// GenVec.last_free points to the last empty entry, there will always be one
// (a new one is made in case there isn't)
// GenEntryFree.next_free points to the next free in the chain after the current one
// if there is no next free entry, the next_free field will point to itself.
//
// those fields are created when members are removed, and used when members are pushed,
// on remove:
//
// 	when an element is removed, it is replaced by a new free entry, when that happens,
// 	the next_free of the entry is set to GenVec.last_free, and GenVec.last_free is set
// 	to the index of the entry, this way a chain of frees is built as elements get removed
//
//  when an element is pushed, it will replace the entry at index of GenVec.last_free,
//  and GenVec.last_free is updated with the next_free of the replaced free entry.
//  In the case where the LAST free entry is replaced by the element (the next_free on
//  this entry points to itself), then, a new free entry is added at the end of the vec
//  to always have GenVec.last_free pointing to something. This way emptys are slowely
//  filled back with elements.
//
// * by indirectly here, I am refering to the fact that no entries are actually stored,
// all there is in the vec is a size_t and the actuall data stored head to tail, entries
// are simply a nicer representation of theses. For how the next_free is stored, it is
// put in the same place as generation, on the size_t, with the first (most significant)
// bit indicating if the entry is free or not.

struct GenIndex {
	size_t index;
	size_t generation;
};

typedef enum {
	Free,
	Occupied,
	Outofrange
} GenEntryState;

struct GenEntryAny {
	GenEntryState state;
	size_t info;
};
struct GenEntryFree {
	GenEntryState state;
	size_t next_free;
};
struct GenEntryOccupied {
	GenEntryState state;
	size_t generation;
	void* data;
};

typedef struct GenEntryAny GenEntryAny;
typedef struct GenEntryFree GenEntryFree;
typedef struct GenEntryOccupied GenEntryOccupied;
typedef struct GenVec GenVec;
typedef struct GenIndex GenIndex;

// keep in mind that, while this struct exists, it is made from the data found
// in the elements of the vector following the memory layout shown above, no
// GenEntry are actually stored.
union GenEntry {
	GenEntryAny any;
	GenEntryFree free;
	GenEntryOccupied occupied;
};

typedef union GenEntry GenEntry;


GenEntry size_t_to_entry(size_t* a);
size_t entry_to_size_t(GenEntry a);
size_t make_new_free(size_t next_free);
size_t make_new_occupied(size_t generation);
GenVec genvec_new(size_t init_size, size_t data_size);
GenIndex genvec_push(GenVec* gv, void* data);
void genvec_remove(GenVec* gv, GenIndex gi, void* data);
GenEntry genvec_get_entry(GenVec* gv, size_t index);
GenIndex genvec_get_index(GenVec* gv, size_t index);
bool genvec_owns(GenVec* gv, GenIndex gi);
bool genvec_has(GenVec* gv, size_t index);
size_t genvec_length(GenVec* gv);
void* genvec_get(GenVec* gv, GenIndex gi);
void genvec_free(GenVec gv);

#endif
