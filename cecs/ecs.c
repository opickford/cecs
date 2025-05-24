#include "ecs.h"

#include "components.h"

#include <string.h>
#include <stdlib.h>

void ECS_init(ECS* ecs)
{
    memset(ecs, 0, sizeof(ECS));
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
