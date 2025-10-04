#ifndef ECS_H
#define ECS_H

// TODO: Forward declarations?
#include "archetype.h"
#include "component.h"
#include "entity.h"
#include "view.h"

#include <chds/vector.h>

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
// TODO: Currently all views must be registered before entities are created,
//       we should allow for new views to be created at any point.
ViewID ECS_view(ECS* ecs, ComponentsBitset include, ComponentsBitset exclude);

// TODO: These will be refactored to use iterators.
void* ECS_get_component_list(ECS* ecs, ArchetypeID aid, ComponentID cid);
int ECS_archetype_num_entities(const ECS* ecs, ArchetypeID aid);

// Entity API
// TODO: Create/destroy? 
EntityID ECS_create_entity(ECS* ecs);
void ECS_destroy_entity(ECS* ecs, EntityID id);



#endif