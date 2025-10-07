#ifndef CECS_H
#define CECS_H

// TODO: Forward declarations?
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "view.h"

typedef struct cecs_t cecs_t;

// TODO: Terminology should be refactored to use table like names: 
//       column, row, field etc.

// To ensure that all entities have an archetype, we must first 
// create an empty archetype.
#define EMPTY_ARCHETYPE_ID 0

// TODO: Comments for public usage.

// cecs_t API
cecs_t* cecs_create();
// TODO: cecs_destroy(cecs_t* ecs);

// Component API
cecs_component_id_t cecs_regsiter_component(cecs_t* ecs, uint32_t component_size);

void* cecs_add_component(cecs_t* ecs, cecs_entity_id_t eid, cecs_component_id_t cid);
void cecs_remove_component(cecs_t* ecs, cecs_entity_id_t eid, cecs_component_id_t cid);
void* cecs_get_component(cecs_t* ecs, cecs_entity_id_t eid, cecs_component_id_t cid);

// cecs_view_t API
cecs_view_id_t cecs_view(cecs_t* ecs, cecs_components_bitset_t include, cecs_components_bitset_t exclude);
cecs_view_iter_t cecs_view_iter(const cecs_t* ecs, cecs_view_id_t vid);

// TODO: Rename cecs_view_tIter_next? Then it would be nice to go in view.h but can't
//       really right?
int cecs_view_iter_next(cecs_view_iter_t* it);
void* cecs_get_column(cecs_view_iter_t it, cecs_component_id_t cid);

// Entity API
cecs_entity_id_t cecs_create_entity(cecs_t* ecs);
void cecs_destroy_entity(cecs_t* ecs, cecs_entity_id_t id);

#endif