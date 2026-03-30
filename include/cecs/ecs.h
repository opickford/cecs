#ifndef CECS_H
#define CECS_H

// TODO: Forward declarations?
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "view.h"

typedef struct CECS CECS;

// TODO: Terminology should be refactored to use table like names: 
//       column, row, field etc.

// To ensure that all entities have an archetype, we must first 
// create an empty archetype.
#define EMPTY_ARCHETYPE_ID 0

// TODO: Comments for public usage.

// CECS API
CECS* cecs_create();
// TODO: cecs_destroy(CECS* ecs);

// Component API
CECS_ComponentId cecs_register_component(CECS* ecs, uint32_t component_size);

void* cecs_add_component(CECS* ecs, CECS_EntityId eid, CECS_ComponentId cid);
void cecs_remove_component(CECS* ecs, CECS_EntityId eid, CECS_ComponentId cid);
void* cecs_get_component(CECS* ecs, CECS_EntityId eid, CECS_ComponentId cid);

// CECS_View API
CECS_ViewId cecs_view_create(CECS* ecs, CECS_ComponentsBitset include, CECS_ComponentsBitset exclude);
CECS_ViewIter cecs_view_iter_create(const CECS* ecs, CECS_ViewId vid);

// TODO: Rename cecs_view_tIter_next? Then it would be nice to go in view.h but can't
//       really right?
int cecs_view_iter_next(CECS_ViewIter* it);
void* cecs_get_column(CECS_ViewIter it, CECS_ComponentId cid);

// Entity API
CECS_EntityId cecs_create_entity(CECS* ecs);
void cecs_destroy_entity(CECS* ecs, CECS_EntityId id);

#endif