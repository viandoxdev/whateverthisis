#include "ecs.h"
#include "genvec.h"
#include "hashmap.h"
#include "vector.h"
#include <endian.h>

/*void memset_pattern(void* dest, size_t dest_size, void* pattern, size_t pattern_size) {
    if(pattern_size > dest_size) {
        memcpy(dest, pattern, dest_size);
    } else {
        size_t ptn = pattern_size; // the total size we filled
        size_t lds = dest_size - ptn; // what is left unfilled
        // first exponentially fill dest with bigger and bigger repeating chunks of the pattern
        // after that lds < ptn (because we stop filling for that precise reason)
        memcpy(dest, pattern, pattern_size);
        while(ptn <= lds) {
            memcpy(dest + ptn, dest, ptn);
            lds -= ptn;
            ptn *= 2;
        }
        if(lds > 0) {
            // lds < ptn, therefor, we can just copy from dest to lds and expect to get the full pattern
            // + whats needed to finish dest_size
            memcpy(dest + dest_size - lds, dest, lds);
        }
    }
}*/

Ecs ecs_new() {
	Ecs res = {};
	res.components = hashmap_new(sizeof(GenVec));
	res.registered_components = vector_new(5, sizeof(ComponentClass));
	return res;
}

void ecs_register_component_class(Ecs* ecs, ComponentClass cclass, size_t component_size) {
	GenVec v = genvec_new(10, component_size);
	if(ecs->registered_components.used > 0) {
		// get the genvec of the first registered component class
		GenVec* model = hashmap_get_hash(&ecs->components, *(ComponentClass*)vector_get_ptr(&ecs->registered_components, 0));
    vector_ensure_allocated(&v.vec, model->vec.used);
    for(int i = 0; i < model->vec.used; i++) {
        GenEntry entry = genvec_get_entry(model, i);
        *(size_t*)vector_get_ptr(&v.vec, i) = entry_to_size_t(entry); // copy the entries over
        if(entry.any.state == Occupied) { // if the entry is occupied make sure cclas is set to 0 (= no component)
            *(ComponentClass*)(vector_get_ptr(&v.vec, i) + sizeof(size_t)) = 0;
        }
    }
    v.generation = model->generation;
    v.last_free = model->last_free; 
	}
	hashmap_set_hash(&ecs->components, cclass, &v);
	vector_push(&ecs->registered_components, &cclass);
}
void ecs_build_entity(Ecs* ecs, EntityBuilder builder) {
    for(int i = 0; i < ecs->registered_components.used; i++) {
        ComponentClass current = *(ComponentClass*)vector_get_ptr(&ecs->registered_components, i);
        GenVec* v = hashmap_get_hash(&ecs->components, current);
        void* comp_ptr = NULL;
        for(int j = 0; j < builder.component_ptrs.used; j++) {
            if(**(ComponentClass**)vector_get_ptr(&builder.component_ptrs, j) == current) {
                comp_ptr = *(void**)vector_get_ptr(&builder.component_ptrs, j);
                break;
            }
        }
        genvec_push(v, comp_ptr);
    }
}
ComponentList ecs_get_components(Ecs* ecs, Entity e) {
   ComponentList res = hashmap_new(sizeof(void*));
    for(int i = 0; i < ecs->registered_components.used; i++) {
        ComponentClass current = *(ComponentClass*)vector_get_ptr(&ecs->registered_components, i);
        GenVec* v = hashmap_get_hash(&ecs->components, current);
        void* comp = genvec_get(v, e);
        if(*(ComponentClass*)comp == 0) continue;
        hashmap_set_hash(&res, current, &comp);
    }
    return res;
}

LocalEntity ecs_copy_entity(Ecs* ecs, Entity e) {
    
}
