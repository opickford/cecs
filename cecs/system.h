#ifndef SYSTEM_H
#define SYSTEM_H

#include "entity.h"
#include "components.h"

#include <string.h>

/*

A system must store a list of entites that have its signature.

TODO: We need some sort of manager to manage the entities here.
      Should entities be an arena? Or just contain max entities?

*/

typedef struct
{
    ComponentsSignature signature;

    EntityID* entities;


} System;

void System_init(System* system)
{
    memset(system, 0, sizeof(System));
}

#endif