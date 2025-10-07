#ifndef CECS_SYSTEM_H
#define CECS_SYSTEM_H

#include "archetype.h"

// TODO: COmment not implementation but how the user should use it!!!!

typedef uint8_t cecs_view_id_t;
#define INVALID_VIEW UINT8_MAX

typedef struct cecs_view_t cecs_view_t;
typedef struct cecs_t cecs_t;

typedef struct
{
    const cecs_t* ecs;
    cecs_view_id_t vid;

    cecs_archetype_id_t* aid;      // Current archetype.
    uint32_t rem;          // Remaining elements to iterate through.

    uint32_t num_entities; // Current number of entities in archetype.

} cecs_view_iter_t;



#endif