#ifndef CECS_ARCHETYPE_INTERNAL_H
#define CECS_ARCHETYPE_INTERNAL_H

#include "archetype.h"

#include "entity.h"
#include "component.h"

#include <chds/vec.h>

// Stores columns for entities matching a signature.
typedef struct cecs_archetype_t
{
    cecs_components_signature_t signature;

    // TODO: Some sort of map?
    chds_vec(cecs_entity_id_t) index_to_entity;

    void** columns;

} cecs_archetype_t;

// TODO: A bit misleading as the cecs_t actually properly initialises this.
void cecs_archetype_init(cecs_archetype_t* archetype);
void cecs_archetype_destroy(cecs_archetype_t* archetype);
void* cecs_archetype_get_column(cecs_archetype_t* archetype, cecs_component_id_t cid);

#endif