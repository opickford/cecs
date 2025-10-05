#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"

// TODO: COmment not implementation but how the user should use it!!!!

typedef uint8_t ViewID;
#define INVALID_VIEW UINT8_MAX

typedef struct View View;
typedef struct ECS ECS;

typedef struct
{
    const ECS* ecs;
    ViewID vid;

    ArchetypeID* aid;      // Current archetype.
    uint32_t rem;          // Remaining elements to iterate through.

    uint32_t num_entities; // Current number of entities in archetype.

} ViewIter;



#endif