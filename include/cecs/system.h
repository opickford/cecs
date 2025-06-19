#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"
#include "component.h"

#include <string.h>

/*
TODO:

System must store the archetypes it cares about, the ecs can manage this for us.

*/

typedef uint8_t SystemID;

typedef struct
{
    ArchetypeID* archetype_ids;
    int num_archetypes;

    ComponentsBitset components_bitset;

} System;

inline void System_init(System* system)
{
    memset(system, 0, sizeof(System));
}

inline void System_add_archetype(System* system, ArchetypeID archetype_id)
{
    // TODO: again, all of this stuff could use some vector implementation
    //       or just grow array by a factor to avoid allocs.

    ArchetypeID* temp_archetype_ids = realloc(system->archetype_ids,
        (system->num_archetypes + 1) * sizeof(ArchetypeID));

    if (!temp_archetype_ids)
    {
        // TODO: Handle failure.
        return;
    }

    system->archetype_ids = temp_archetype_ids;

    system->archetype_ids[system->num_archetypes] = archetype_id;
    ++system->num_archetypes;
}

inline void System_destroy(System* system)
{
    free(system->archetype_ids);
}

#endif