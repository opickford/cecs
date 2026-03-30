#ifndef CECS_ARCHETYPE_INTERNAL_H
#define CECS_ARCHETYPE_INTERNAL_H

#include "archetype.h"

#include "entity.h"
#include "component_internal.h"

#include <chds/vec.h>

// Stores columns for entities matching a signature.
typedef struct CECS_Archetype
{
    CECS_ComponentsSignature signature;

    // TODO: Some sort of map?
    CHDS_Vec(CECS_EntityId) index_to_entity;

    void** columns;

} CECS_Archetype;

// TODO: A bit misleading as the CECS actually properly initialises this.
void cecs_archetype_init(CECS_Archetype* archetype);
void cecs_archetype_destroy(CECS_Archetype* archetype);
void* cecs_archetype_get_column(CECS_Archetype* archetype, CECS_ComponentId cid);

#endif