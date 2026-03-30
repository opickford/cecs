#ifndef CECS_COMPONENT_INTERNAL_H
#define CECS_COMPONENT_INTERNAL_H

#include "component.h"

// Private component internals

// TODO: Document.......... why all this......

// TODO: Should ID go in here?
typedef struct
{
    CECS_ComponentId id; // TODO: Pretty sure we don't want this? Not sure.
    uint32_t size;
} CECS_ComponentInfo;

// TODO: We don't need include/exclude here right?
// TODO: Also is this really just an cecs_archetype_tSignature?
typedef struct
{
    CECS_ComponentsBitset bitset;

    // TODO: CHDS_VEC?
    // TODO: If we use a CHDS_VEC then some sort of shrink to fit function
    //       could be nice, unless we can reserve before??
    int num_components;
    CECS_ComponentInfo* infos;

} CECS_ComponentsSignature;

#endif