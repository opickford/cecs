#ifndef ECS_H
#define ECS_H

// TODO: Forward declarations?
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "view.h"

typedef struct ECS ECS;

// TODO: Terminology should be refactored to use table like names: 
//       column, row, field etc.

// To ensure that all entities have an archetype, we must first 
// create an empty archetype.
#define EMPTY_ARCHETYPE_ID 0

// TODO: Comments for public usage.

// ECS API
ECS* ECS_create();
// TODO: ECS_destroy(ECS* ecs);

// Component API
ComponentID ECS_register_component(ECS* ecs, uint32_t component_size);

void* ECS_add_component(ECS* ecs, EntityID eid, ComponentID cid);
void ECS_remove_component(ECS* ecs, EntityID eid, ComponentID cid);
void* ECS_get_component(ECS* ecs, EntityID eid, ComponentID cid);

// View API
ViewID ECS_view(ECS* ecs, ComponentsBitset include, ComponentsBitset exclude);
ViewIter ECS_view_iter(const ECS* ecs, ViewID vid);

// TODO: Rename ViewIter_next? Then it would be nice to go in view.h but can't
//       really right?
int ECS_view_iter_next(ViewIter* it);
void* ECS_get_column(ViewIter it, ComponentID cid);

// Entity API
EntityID ECS_create_entity(ECS* ecs);
void ECS_destroy_entity(ECS* ecs, EntityID id);

#endif