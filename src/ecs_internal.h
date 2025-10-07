#ifndef CECS_INTERNAL_H
#define CECS_INTERNAL_H

#include "ecs.h"

#include "component_internal.h"

#include <chds/vec.h>

typedef struct
{
    cecs_archetype_id_t archetype_id;
    int column;
} cecs_entity_index_t;

typedef struct cecs_t
{
    // Entities
    int num_used_entities;

    cecs_entity_id_t* free_entities;
    int free_entities_count;
    int free_entities_capacity;

    cecs_components_bitset_t* entity_components_bitsets;

    // Stores the archetype that the entity belongs to and the position in that
    // archetype.
    cecs_entity_index_t* entity_indices;

    chds_vec(cecs_component_info_t) component_infos;
    chds_vec(cecs_archetype_t) archetypes;
    chds_vec(cecs_view_t) views;

} cecs_t;

#endif