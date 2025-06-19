#ifndef ECS_H
#define ECS_H

#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "system.h"

// TODO: Just redo everything really.

typedef struct
{
    ArchetypeID archetype_id;
    int component_list_index;
} EntityIndex;

typedef struct
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

    // Components
    int num_components;
    ComponentInfo* component_infos;

    // Archetypes
    int num_archetypes;
    Archetype* archetypes;

    // Systems
    int num_systems;
    System* systems;

} ECS;

void ECS_init(ECS* ecs);

ComponentID ECS_register_component(ECS* ecs, uint32_t component_size);
SystemID ECS_register_system(ECS* ecs);

// TODO: Create/destroy? 
EntityID ECS_create_entity(ECS* ecs);
void ECS_destroy_entity(ECS* ecs, EntityID id);

void ECS_add_component(ECS* ecs, EntityID eid, ComponentID cid);
void ECS_remove_component(ECS* ecs, EntityID eid, ComponentID cid);
void* ECS_get_component(ECS* ecs, EntityID eid, ComponentID cid);

// Internal helper functions.
ArchetypeID ECS_create_archetype(ECS* ecs, ComponentsBitset archetype_bitset);
void ECS_move_archetype(ECS* ecs, EntityID id, ArchetypeID old_archetype_id, 
    ArchetypeID new_archetype_id);

void Archetype_add_entity(const ECS* ecs, Archetype* archetype, EntityID eid);
void Archetype_remove_entity(const ECS* ecs, Archetype* archetype, int entity_index);

#endif