#ifndef __ECS_H
#define __ECS_H
#include "vector.h"
#include "genvec.h"
#include "hashmap.h"

#define MAKE_COMPONENT(name, members) \
struct name##Component { \
	AbstractComponent base; \
	members \
}; \
typedef struct name##Component name##Component; \



typedef unsigned int ComponentClass;

struct ComponentPointer {
	void* ptr;
	ComponentClass comp_class;
};
typedef struct ComponentPointer ComponentPointer;


struct EntityBuilder {
	Vector component_ptrs;
};
typedef struct EntityBuilder EntityBuilder;

struct AbstractComponent {
	ComponentClass comp_class;
};
typedef struct AbstractComponent AbstractComponent;

struct LocalEntity {
	Vector components;
	void* data;
};
typedef struct LocalEntity LocalEntity;

struct Ecs {
	HashMap components;
};
typedef struct Ecs Ecs;

EntityBuilder entity_builder_new();
void entity_builder_add_component();
LocalEntity entity_build_local();

#endif
