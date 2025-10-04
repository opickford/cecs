#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"

// TODO: COmment not implementation but how the user should use it!!!!

typedef uint8_t ViewID;

// TODO: Some function for passing in component ids separately.

typedef struct View View;
typedef struct ECS ECS;

// TODO: Needs refactor.

typedef struct
{
    const ECS* ecs;

    ViewID vid;

    uint32_t current;
    uint32_t end;

    ArchetypeID* aid;

    uint32_t num_entities;

} ViewIter;



#endif