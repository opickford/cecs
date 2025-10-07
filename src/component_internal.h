#ifndef CECS_COMPONENT_INTERNAL_H
#define CECS_COMPONENT_INTERNAL_H

#include "component.h"

// Private component internals

// TODO: Document.......... why all this......

// TODO: Should ID go in here?
typedef struct
{
    cecs_component_id_t id; // TODO: Pretty sure we don't want this? Not sure.
    uint32_t size;
} cecs_component_info_t;

// TODO: We don't need include/exclude here right?
// TODO: Also is this really just an cecs_archetype_tSignature?
typedef struct
{
    cecs_components_bitset_t bitset;

    // TODO: chds_vec?
    // TODO: If we use a chds_vec then some sort of shrink to fit function
    //       could be nice, unless we can reserve before??
    int num_components;
    cecs_component_info_t* infos;

} cecs_components_signature_t;

#endif