#ifndef CECS_COMPONENT_INTERNAL_H
#define CECS_COMPONENT_INTERNAL_H

#include "component.h"

// Private component internals

// TODO: Document.......... why all this......

// TODO: Should ID go in here?
typedef struct
{
    cecs_component_id id; // TODO: Pretty sure we don't want this? Not sure.
    uint32_t size;
} cecs_component_info;

// TODO: We don't need include/exclude here right?
// TODO: Also is this really just an cecs_archetype_tSignature?
typedef struct
{
    cecs_components_bitset bitset;

    // TODO: CHDS_VEC?
    // TODO: If we use a CHDS_VEC then some sort of shrink to fit function
    //       could be nice, unless we can reserve before??
    int num_components;
    cecs_component_info* infos;

} cecs_components_signature;

#endif