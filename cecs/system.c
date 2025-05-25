#include "system.h"

#include <stdlib.h>
#include <stdio.h>

void System_add_entity(System* system, EntityID id)
{
    // Increase capacity if necessary.
    if (system->num_entities == system->capacity)
    {
        ++system->capacity;

        EntityID* temp = realloc(system->entities, system->capacity * sizeof(EntityID));
        if (!temp)
        {
            printf("Failed to realloc for system entities.\n");
            return;
        }
        system->entities = temp;
    }

    system->entities[system->num_entities] = id;
    ++system->num_entities;
}

void System_remove_entity(System* system, EntityID id)
{
    // TODO: For this to be O(1) we need a map of id to index.
    //       this is annoying though as we then need to grow this array
    //       when a new entity is added. - or an actual map/set data structure. 
    //       I feel like a map for this sort of thing would be pretty simple.

    //       For now, just loop through until we find our entity id, if a
    //       performance issue, sort later.

    // Assume entity is part of system already, otherwise wasted loop.
    for (int i = 0; i < system->num_entities; ++i)
    {
        if (system->entities[i] == id)
        { 
            // Swap with one at end.
            const EntityID end_id = system->entities[system->num_entities - 1];
            system->entities[i] = end_id;

            --system->num_entities;
            break;
        }
    }

}
