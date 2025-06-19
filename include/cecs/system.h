#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"

/*
TODO:

System must store the archetypes it cares about, the ecs can manage this for us.

*/

typedef struct
{
    Archetype* archetypes;
    int num_archetypes;


} System;

#endif