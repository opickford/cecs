#include "ecs.h"

#include "components.h"

#include <string.h>
#include <stdlib.h>

void ECS_init(ECS* ecs)
{
    memset(ecs, 0, sizeof(ECS));
}

SystemID ECS_add_System(ECS* ecs)
{
    System* temp = realloc(ecs->systems, sizeof(System) * (size_t)(ecs->num_systems + 1));
    if (!temp)
    {
        printf("ECS_add_system failed to realloc.\n");
        return INVALID_SYSTEM_ID;
    }
    ecs->systems = temp;

    SystemID system_id = ecs->num_systems;
    System* system = &ecs->systems[system_id];
    System_init(system);
    ++ecs->num_systems;

    // TODO: To get around using systemids rather than pointers, we could just preallocate
    //       for the number of systems we want.
    return system_id;
}

EntityID ECS_create_entity(ECS* ecs)
{
    EntityID entity;

    ++ecs->num_used_entities;

    // Re-use if free Entity.
    if (ecs->free_entities_count > 0)
    {   
        // Free entities works like a stack.
        entity = ecs->free_entities[ecs->free_entities_count - 1];

        --ecs->free_entities_count;

        // Clear old signature.
        ecs->signatures[entity] = EMPTY_SIGNATURE;
    }
    else
    {
        

        const int total_created_entities = ecs->num_used_entities + ecs->free_entities_count;

        // TODO: Grow by some factor to save allocations?
        
        ComponentsSignature* temp_signatures = realloc(ecs->signatures, total_created_entities * sizeof(ComponentsSignature));
        if (!temp_signatures)
        {
            printf("Failed to alloc for ecs->signatures.\n");
            return INVALID_ENTITY;
        }
        ecs->signatures = temp_signatures;

        entity = total_created_entities - 1;

        ecs->signatures[entity] = EMPTY_SIGNATURE; // Initialise new signature to empty.
    
        
        // Grow sparse arrays for each ComponentList.
        // TODO: If I stored a list of component lists in ECS this would just be a for loop maybe better.
#define X(ComponentT, _) \
{ \
    int* temp_id_to_index = realloc(ecs->##ComponentT##s.id_to_index, total_created_entities * sizeof(int));           \
    if (!temp_id_to_index)                                                                                    \
    {                                                                                                         \
        printf("failed to grow " #ComponentT "s\n");                                                          \
        return INVALID_ENTITY;                                                                                \
    }                                                                                                         \
    ecs->##ComponentT##s.id_to_index = temp_id_to_index;                                                      \
                                                                                                              \
    EntityID* temp_index_to_id = realloc(ecs->##ComponentT##s.index_to_id, total_created_entities * sizeof(EntityID)); \
    if (!temp_index_to_id)                                                                                    \
    {                                                                                                         \
        printf("failed to grow " #ComponentT "s\n");                                                          \
        return INVALID_ENTITY;                                                                                \
    }                                                                                                         \
    ecs->##ComponentT##s.index_to_id = temp_index_to_id;                                                      \
}
        COMPONENTS_LIST
#undef X
    }
    return entity;
}

// TODO: Rename destroy?
void ECS_remove_entity(ECS* ecs, EntityID id)
{
    --ecs->num_used_entities;

    // TODO: test validity of id?

    // Entities don't need to be packed as we will never be iterating over the entities loop.

    // Clear entity signature.
    const ComponentsSignature sig = ecs->signatures[id];
    ecs->signatures[id] = EMPTY_SIGNATURE;

    // Grow capacity if needed.
    if (ecs->free_entities_count == ecs->free_entities_capacity)
    {
        // TODO: Grow by factor?
        ++ecs->free_entities_capacity;

        EntityID* temp_free_entities = realloc(ecs->free_entities, ecs->free_entities_capacity * sizeof(EntityID));
        if (!temp_free_entities)
        {
            printf("failed to grow ecs->free_entities\n");
            return;
        }
        ecs->free_entities = temp_free_entities;
    }

    // Save the new free entity.
    ecs->free_entities[ecs->free_entities_count] = id;
    ++ecs->free_entities_count;

    // Remove entity from all systems.
    for (int i = 0; i < ecs->num_systems; ++i)
    {
        System* system = &ecs->systems[i];
        if ((sig & system->signature) == system->signature)
        {
            System_remove_entity(system, id);
        }
    }

    // Try remove all components, if component doesn't exist for entity 
    // it will just be ignored.
#define X(ComponentT, _) ECS_remove_##ComponentT(ecs, id);
    COMPONENTS_LIST
#undef X

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

void ECS_on_remove_component(ECS* ecs, EntityID id, ComponentsSignature component_signature)
{
    // To figure out if we need to remove an entity from a system, we rely on the fact that if
    // it has a component, it must be in a system that requires that component.
    const ComponentsSignature entity_sig = ecs->signatures[id];
    const ComponentsSignature previous_entity_sig = entity_sig | component_signature;

    for (int i = 0; i < ecs->num_systems; ++i)
    {
        System* system = &ecs->systems[i];
        const ComponentsSignature system_sig = system->signature;
        
        // 1) We only care about system's that require the component we just removed.
        // 2) The old signature must match the system's (otherwise the entity isn't in the system).
        // 3) The new signature must not match the system's (a required component was removed).
        if ((system_sig & component_signature) && 
            ((previous_entity_sig & system_sig) == system_sig) && 
            ((entity_sig & system_sig) != system_sig))
        {
            System_remove_entity(system, id);
        }
    }
}
