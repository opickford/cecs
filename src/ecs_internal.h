#ifndef ECS_INTERNAL_H
#define ECS_INTERNAL_H

#include "ecs.h"

#include <chds/vector.h>

typedef struct
{
    ArchetypeID archetype_id;
    int column;
} EntityIndex;

typedef struct ECS
{
    // Entities
    int num_used_entities;

    EntityID* free_entities;
    int free_entities_count;
    int free_entities_capacity;

    ComponentsBitset* entity_components_bitsets;

    // Stores the archetype that the entity belongs to and the position in that
    // archetype.
    EntityIndex* entity_indices;

    Vector(ComponentInfo) component_infos;
    Vector(Archetype) archetypes;
    Vector(View) views;

} ECS;

#endif