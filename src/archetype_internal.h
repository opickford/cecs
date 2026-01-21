#ifndef CECS_ARCHETYPE_INTERNAL_H
#define CECS_ARCHETYPE_INTERNAL_H

#include "archetype.h"

#include "entity.h"
#include "component_internal.h"

#include <chds/vec.h>

// Stores columns for entities matching a signature.
typedef struct cecs_archetype
{
    cecs_components_signature signature;

    // TODO: Some sort of map?
    CHDS_VEC(cecs_entity_id) index_to_entity;

    void** columns;

} cecs_archetype;

// TODO: A bit misleading as the cecs actually properly initialises this.
void cecs_archetype_init(cecs_archetype* archetype);
void cecs_archetype_destroy(cecs_archetype* archetype);
void* cecs_archetype_get_column(cecs_archetype* archetype, cecs_component_id cid);

#endif