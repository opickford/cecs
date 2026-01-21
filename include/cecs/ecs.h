#ifndef CECS_H
#define CECS_H

// TODO: Forward declarations?
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "view.h"

typedef struct cecs cecs;

// TODO: Terminology should be refactored to use table like names: 
//       column, row, field etc.

// To ensure that all entities have an archetype, we must first 
// create an empty archetype.
#define EMPTY_ARCHETYPE_ID 0

// TODO: Comments for public usage.

// cecs API
cecs* cecs_create();
// TODO: cecs_destroy(cecs* ecs);

// Component API
cecs_component_id cecs_register_component(cecs* ecs, uint32_t component_size);

void* cecs_add_component(cecs* ecs, cecs_entity_id eid, cecs_component_id cid);
void cecs_remove_component(cecs* ecs, cecs_entity_id eid, cecs_component_id cid);
void* cecs_get_component(cecs* ecs, cecs_entity_id eid, cecs_component_id cid);

// cecs_view API
cecs_view_id cecs_view_create(cecs* ecs, cecs_components_bitset include, cecs_components_bitset exclude);
cecs_view_iter cecs_view_iter_create(const cecs* ecs, cecs_view_id vid);

// TODO: Rename cecs_view_tIter_next? Then it would be nice to go in view.h but can't
//       really right?
int cecs_view_iter_next(cecs_view_iter* it);
void* cecs_get_column(cecs_view_iter it, cecs_component_id cid);

// Entity API
cecs_entity_id cecs_create_entity(cecs* ecs);
void cecs_destroy_entity(cecs* ecs, cecs_entity_id id);

#endif