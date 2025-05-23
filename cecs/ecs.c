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
    }
    else
    {
        {
            ecs->count++;
            ecs->capacity++;

            EntityID* temp = realloc(ecs->entities, ecs->capacity * sizeof(EntityID));
            if (!temp)
            {
                printf("FAILED ALLOC FOR ENTITY*.\n");
                return INVALID_ENTITY;
            }
            ecs->entities = temp;
            entity = ecs->count;
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
