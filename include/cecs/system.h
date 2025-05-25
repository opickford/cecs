#ifndef SYSTEM_H
#define SYSTEM_H

#include "entity.h"
#include "components.h"

#include <string.h>
#include <stdint.h>

/*

A system must store a list of entites that have its signature.

Note, we don't need to look out for duplicate entities, we're going off the assumption
that we will always check the entity signature before adding to a system.

TODO: We need some sort of manager to manage the entities here.
      Should entities be an arena? Or just contain max entities?

      We need to keep the entities array packed. So 

*/

typedef uint32_t SystemID;
#define INVALID_SYSTEM_ID UINT32_MAX

typedef struct
{
    ComponentsSignature signature;

    EntityID* entities;
    int num_entities;
    int capacity;

} System;

inline void System_init(System* system)
{
    memset(system, 0, sizeof(System));
}

// TODO: These should be called by the ECS, not by a user.
void System_add_entity(System* system, EntityID id);
void System_remove_entity(System* system, EntityID id);




#endif