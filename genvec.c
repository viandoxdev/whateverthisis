#include "genvec.h"

#define gentype GENVEC_GENTYPE

GenEntry size_t_to_entry(size_t* a) {
	GenEntry res = {};
	bool occupied = ((*a) & GENVEC_GENOCCUPIED_BIT) == 0;
	// this also sets res.occupied.generation and res.free.next_free, since those have the same offset and size 
	res.any.info = (*a) & ~GENVEC_GENOCCUPIED_BIT;
	if(occupied) {
		res.occupied.state = Occupied;
		res.occupied.data = a + sizeof(size_t);
	} else {
		res.occupied.state = Free;
	}
	return res;
}

size_t entry_to_size_t(GenEntry a) {
	if(a.any.state == Outofrange) return 0;
	size_t res = 0;
	// set occupation bit
	if(a.any.state == Free) res |= GENVEC_GENOCCUPIED_BIT;
	// set info
	res |= a.any.info & ~GENVEC_GENOCCUPIED_BIT;
	return res;
}

// while the more general entry_to_size_t(GenEntryFree) works, this is much faster. (ans less verbose)
size_t make_new_free(size_t next_free) {
	return next_free | GENVEC_GENOCCUPIED_BIT;
}
size_t make_new_occupied(size_t generation) {
	// could be:
	// return generation & ~GENVEC_GENOCCUPIED_BIT;
	// but it honestly wouldn't change a thing
	return generation;
}

GenVec genvec_new(size_t init_size, size_t data_size) {
	GenVec res = {};
	res.generation = 0;
	res.vec = vector_new(init_size, sizeof(size_t) + data_size);
	res.data_size = data_size;
	// see genvec.h comments for more info
	res.last_free = 0;
	// res.vec.used is 0 after vector_new, this always gives 1
	vector_ensure_allocated(&res.vec, ++res.vec.used);
	// this, firsts, get the pointer to the first member of the vec
	// then casts it as size_t* (see memory layout) and assigns GENVEC_GENOCCUPIED_BIT
	// to it, this makes a empty entry with a next_free pointing to itself at index 0
	*((size_t*)vector_get_ptr(&res.vec, 0)) = GENVEC_GENOCCUPIED_BIT;
	return res;
}

void genvec_push(GenVec* gv, void* data) {
	// last_free is guarenteed to point at a free entry
	GenEntryFree last_free = genvec_get_entry(gv, gv->last_free).free;
	void* slot_ptr = vector_get_ptr(&gv->vec, gv->last_free);
	void* data_ptr = slot_ptr  + sizeof(size_t);
	// set info in slot
	*((size_t*)slot_ptr) = make_new_occupied(gv->generation);
	// set data
	memcpy(data_ptr, data, gv->data_size);
	// if last_free points to itself
	if(last_free.next_free == gv->last_free) {
		// make room for new free
		vector_ensure_allocated(&gv->vec, ++gv->vec.used);
		size_t new_free = make_new_free(gv->vec.used - 1);
		// put new_free in array
		*((size_t*)vector_get_ptr(&gv->vec, gv->vec.used - 1)) = new_free;
	} else {
		// if it doesn't just get the next free
		gv->last_free = last_free.next_free;
	}
}

void genvec_remove(GenVec* gv, GenIndex gi, void* data) {
	// element doesn't exist / is already gone
	if(!genvec_owns(gv, gi)) return;
	if(data != NULL) {
		// if genvec_owns(gv, gi) then the entry has to be occupied
		GenEntryOccupied entry = genvec_get_entry(gv, gi.index).occupied;
		memcpy(data, entry.data, gv->data_size);
	}
	size_t new_free = make_new_free(gv->last_free);
	gv->last_free = gi.index;
	*((size_t*)vector_get_ptr(&gv->vec, gi.index)) = new_free;
	// the one thing that makes it all worth
	gv->generation++;
}

GenEntry genvec_get_entry(GenVec* gv, size_t index) {
	if(!vector_has(&gv->vec, index)) {
		GenEntry res = {};
		res.any.state = Outofrange;
		return res;
	}

	size_t* gen_ptr = vector_get_ptr(&gv->vec, index);
	return size_t_to_entry(gen_ptr);
}

GenIndex genvec_get_index(GenVec* gv, size_t index) {
	GenIndex res = {};
	if(genvec_has(gv, index)) {
		// genvec_has only returns true if the entry is occupied
		GenEntryOccupied entry = genvec_get_entry(gv, index).occupied;
		res.generation = entry.generation;
		res.index = index;
		return res;
	} else {
		return res;
	}
}

bool genvec_owns(GenVec* gv, GenIndex gi) {
	GenEntry entry = genvec_get_entry(gv, gi.index);
	return entry.any.state == Occupied && entry.occupied.generation == gi.generation;
}

bool genvec_has(GenVec* gv, size_t index) {
	GenEntry entry = genvec_get_entry(gv, index);
	return entry.any.state == Occupied;
}

size_t genvec_length(GenVec* gv) {
	return gv->vec.used;
}
