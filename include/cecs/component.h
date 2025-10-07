#ifndef CECS_COMPONENT_H
#define CECS_COMPONENT_H

#include <stdint.h>

// TODO: What is private here and what isn't.

/*

TODO: Probably gotta rethink all of this at some point.
// TODO: Write this out better anyways, don't need docs at top just above
         types etc.

cecs_component_id_t
- The cecs_t stores an array of components, hence, cecs_component_id_t is an index into that array.
- TODO: Static sized array of components in ecs?

ComponentBitset:
- Represents flags for present components, this is for quicker comparisons.

cecs_component_info_t: 
- Stores the sizeof(ComponentT) and it's ID (index in ecs components array).

cecs_components_signature_t:
- Stores a bitset and array of component infos.

*/

// Represents a bitset of components.
// TODO: Issue this only allows for 16 components. 
//       In the future we could probably expand this to some array of bitsets.
//       but that should be in the CHDS lib?
typedef uint16_t cecs_components_bitset_t; // TODO: Just signature?
typedef uint8_t cecs_component_id_t;

#define CECS_MAX_COMPONENTS 16
#define CECS_EMPTY_COMPONENTS_BITSET 0

// TODO: Should this be cast to something?
#define CECS_COMPONENT_ID_TO_BITSET(id) (1 << id)



// TODO: These are private right????? so we can/should/shouldnt? remove cecs prefix?

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