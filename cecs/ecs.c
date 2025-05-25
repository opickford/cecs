#include "ecs.h"

#include "components.h"

#include <string.h>
#include <stdlib.h>

void ECS_init(ECS* ecs)
{
    memset(ecs, 0, sizeof(ECS));
}

System* ECS_add_System(ECS* ecs)
{
    System* temp = realloc(ecs->systems, sizeof(System) * (ecs->num_systems + 1));
    if (!temp)
    {
        printf("ECS_add_system failed to realloc.\n");
        return;
    }
    ecs->systems = temp;

    System* system = &ecs->systems[ecs->num_systems];
    System_init(system);
    ++ecs->num_systems;

    return system;
}

EntityID ECS_create_entity(ECS* ecs)
{
    EntityID entity;

    // Re-use if free Entity.
    if (ecs->count < ecs->capacity)
    {
        ecs->count++;
        
        // Free entities works like a stack.
        entity = ecs->free_entities[ecs->capacity - ecs->count];

        // Clear old signature.
        ecs->signatures[entity] = EMPTY_SIGNATURE;
    }
    else
    {
        // TODO: Grow by some factor to save allocations?
        {
            ecs->count++;
            ecs->capacity++;

            EntityID* temp_entities = realloc(ecs->entities, ecs->capacity * sizeof(EntityID));
            if (!temp_entities)
            {
                printf("Failed to alloc for ecs->entities.\n");
                return INVALID_ENTITY;
            }
            
            ecs->entities = temp_entities;

            ComponentsSignature* temp_signatures = realloc(ecs->signatures, ecs->capacity * sizeof(ComponentsSignature));
            if (!temp_signatures)
            {
                printf("Failed to alloc for ecs->signatures.\n");
                return INVALID_ENTITY;
            }
            ecs->signatures = temp_signatures;
            ecs->signatures[ecs->count - 1] = EMPTY_SIGNATURE; // Initialise new signature to empty.

            entity = ecs->count - 1;
        }
        
        // Grow sparse array for each ComponentList.
#define X(ComponentT, _) \
{ \
    int* temp = realloc(ecs->##ComponentT##s.id_to_index, ecs->capacity * sizeof(int)); \
    if (!temp) \
    { \
        printf("failed to grow " #ComponentT "s\n"); \
        return INVALID_ENTITY; \
    } \
    ecs->##ComponentT##s.id_to_index = temp; \
}
        COMPONENTS_LIST
#undef X
    }
    return entity;
}

void ECS_on_add_component(ECS* ecs, EntityID id, ComponentsSignature component_signature)
{
    // To figure out if a system already has an entity, we're just relying on the fact
    // that if we added a component to an entity, then it cannot be in a system with that
    // component.

    const ComponentsSignature entity_sig = ecs->signatures[id];

    for (int i = 0; i < ecs->num_systems; ++i)
    {
        System* system = &ecs->systems[i];
        const ComponentsSignature system_sig = system->signature;
        if ((system_sig & component_signature) && ((entity_sig & system_sig) == system_sig))
        {
            System_add_entity(system, id);
        }
    }
}
