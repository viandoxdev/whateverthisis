#ifndef __ECS_H
#define __ECS_H
#include "vector.h"
#include "genvec.h"
#include "hashmap.h"
#include <stddef.h>

typedef size_t ComponentClass;
typedef HashMap ComponentList;
typedef GenIndex Entity;

struct EntityBuilder {
	Vector component_ptrs;
};
typedef struct EntityBuilder EntityBuilder;

struct AbstractComponent {
	ComponentClass cclass;
};
typedef struct AbstractComponent AbstractComponent;

struct LocalEntity {
	Vector components;
	void* data;
};
typedef struct LocalEntity LocalEntity;

struct Ecs {
	HashMap components;
	Vector registered_components;
};
typedef struct Ecs Ecs;


EntityBuilder entity_builder_new();
void entity_builder_add_component();
LocalEntity entity_build_local();

void ecs_register_component_class(Ecs* ecs, ComponentClass cclass, size_t component_size);

inline void* component_get_from_list(ComponentList* list, const char* cclass) {
	return hashmap_get_str(list, (char*)cclass);
}

#endif
