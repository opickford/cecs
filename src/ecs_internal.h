#ifndef CECS_INTERNAL_H
#define CECS_INTERNAL_H

#include "ecs.h"

#include "component_internal.h"

#include <chds/vec.h>

typedef struct
{
    CECS_ArchetypeId archetype_id;
    int column;
} CECS_EntityIndex;

typedef struct CECS
{
    // Entities
    int num_used_entities;

    CECS_EntityId* free_entities;
    int free_entities_count;
    int free_entities_capacity;

    CECS_ComponentsBitset* entity_components_bitsets;

    // Stores the archetype that the entity belongs to and the position in that
    // archetype.
    CECS_EntityIndex* entity_indices;

    CHDS_Vec(CECS_ComponentInfo) component_infos;
    CHDS_Vec(CECS_Archetype) archetypes;
    CHDS_Vec(CECS_View) views;

} CECS;

#endif