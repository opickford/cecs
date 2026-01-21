#ifndef CECS_INTERNAL_H
#define CECS_INTERNAL_H

#include "ecs.h"

#include "component_internal.h"

#include <chds/vec.h>

typedef struct
{
    cecs_archetype_id archetype_id;
    int column;
} cecs_entity_index;

typedef struct cecs
{
    // Entities
    int num_used_entities;

    cecs_entity_id* free_entities;
    int free_entities_count;
    int free_entities_capacity;

    cecs_components_bitset* entity_components_bitsets;

    // Stores the archetype that the entity belongs to and the position in that
    // archetype.
    cecs_entity_index* entity_indices;

    CHDS_VEC(cecs_component_info) component_infos;
    CHDS_VEC(cecs_archetype) archetypes;
    CHDS_VEC(cecs_view) views;

} cecs;

#endif