#ifndef ECS_H
#define ECS_H

// TODO: Forward declarations?
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "view.h"

#include <chds/vector.h>

// TODO: Terminology should be refactored to use table like names: column, row, field etc.

// To ensure that all entities have an archetype, we must first 
// create an empty archetype.
#define EMPTY_ARCHETYPE_ID 0

// TODO: Comments for public functions.

typedef struct
{
    ArchetypeID archetype_id;
    int component_list_index;
} EntityIndex;

// TODO: Move this to the implementation file to hide it????
//       This will stop us from exposing Vector to the user.
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

// ECS API
void ECS_init(ECS* ecs);
// TODO: ECS_destroy(ECS* ecs);

// Component API
ComponentID ECS_register_component(ECS* ecs, uint32_t component_size);

void* ECS_add_component(ECS* ecs, EntityID eid, ComponentID cid);
void ECS_remove_component(ECS* ecs, EntityID eid, ComponentID cid);
void* ECS_get_component(ECS* ecs, EntityID eid, ComponentID cid);

// View API

// TODO: Should these be named ECS_View??? Capitalised as type???
ViewID ECS_view(ECS* ecs, ComponentsBitset include, ComponentsBitset exclude);
ViewIter ECS_view_iter(const ECS* ecs, ViewID vid);
int ECS_view_iter_next(ViewIter* it);

// TODO: These will be refactored to use iterators.
void* ECS_get_component_list(ViewIter it, ComponentID cid);

// Entity API
// TODO: Create/destroy? 
EntityID ECS_create_entity(ECS* ecs);
void ECS_destroy_entity(ECS* ecs, EntityID id);



#endif