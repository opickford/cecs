#ifndef ECS_H
#define ECS_H

#include "entity.h"
#include "component.h"
#include "archetype.h"

// TODO: Just redo everything really.

typedef struct
{
    // Entities
    int num_used_entities;

    EntityID* free_entities;
    int free_entities_count;
    int free_entities_capacity;

    ComponentsBitset* entity_components_bitsets;

    int* entity_indices; // Stores the position of the entity in it's archetype.

    // Components
    int num_components;
    ComponentInfo* component_infos;

    // Archetypes
    int num_archetypes;
    Archetype* archetypes;

} ECS;

void ECS_init(ECS* ecs);

ComponentID ECS_register_component(ECS* ecs, uint32_t component_size);

// TODO: Create/destroy? 
EntityID ECS_create_entity(ECS* ecs);
void ECS_destroy_entity(ECS* ecs, EntityID id);

void ECS_add_component(ECS* ecs, EntityID eid, ComponentID cid);
void ECS_remove_component(ECS* ecs, EntityID eid, ComponentID cid);

// Internal helper functions.
Archetype* ECS_create_archetype(ECS* ecs, ComponentsBitset archetype_bitset);
void ECS_move_archetype(ECS* ecs, EntityID id, Archetype* old_archetype, 
    Archetype* new_archetype);

void Archetype_add_entity(const ECS* ecs, Archetype* archetype, EntityID eid);
void Archetype_remove_entity(const ECS* ecs, Archetype* archetype, int entity_index);

#endif