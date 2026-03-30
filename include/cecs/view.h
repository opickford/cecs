#ifndef CECS_SYSTEM_H
#define CECS_SYSTEM_H

#include "archetype.h"

// TODO: COmment not implementation but how the user should use it!!!!

typedef uint8_t CECS_ViewId;
#define INVALID_VIEW UINT8_MAX

typedef struct CECS_View CECS_View;
typedef struct CECS CECS;

typedef struct
{
    const CECS* ecs;
    CECS_ViewId vid;

    CECS_ArchetypeId* aid;      // Current archetype.
    uint32_t rem;          // Remaining elements to iterate through.

    uint32_t num_entities; // Current number of entities in archetype.

} CECS_ViewIter;



#endif