#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdint.h>

// TODO: What is private here and what isn't.

/*

TODO: Probably gotta rethink all of this at some point.
// TODO: Write this out better anyways, don't need docs at top just above
         types etc.

ComponentID
- The ECS stores an array of components, hence, ComponentID is an index into that array.
- TODO: Static sized array of components in ecs?

ComponentBitset:
- Represents flags for present components, this is for quicker comparisons.

ComponentInfo: 
- Stores the sizeof(ComponentT) and it's ID (index in ecs components array).

ComponentsSignature:
- Stores a bitset and array of component infos.

*/

// Represents a bitset of components.
// TODO: Issue this only allows for 16 components. 
//       In the future we could probably expand this to some array of bitsets.
//       but that should be in the CHDS lib?
typedef uint16_t ComponentsBitset; // TODO: Just signature?
typedef uint8_t ComponentID; // TODO: Rename ids like ComponentId?

#define COMPONENT_BITSET_SIZE 16 // TODO: Rename to MAX COMPONENTS?
#define COMPONENTS_EMPTY_BITSET 0

// TODO: Should this be cast to something?
#define COMPONENT_ID_TO_BITSET(id) (1 << id)





// TODO: Document.......... why all this......

// TODO: Should ID go in here?
typedef struct
{
    ComponentID id; // TODO: Pretty sure we don't want this? Not sure.
    uint32_t size;
} ComponentInfo;

// TODO: We don't need include/exclude here right?
// TODO: Also is this really just an ArchetypeSignature?
typedef struct
{
    ComponentsBitset bitset;

    // TODO: chds_vec?
    // TODO: If we use a chds_vec then some sort of shrink to fit function
    //       could be nice, unless we can reserve before??
    int num_components;
    ComponentInfo* infos;

} ComponentsSignature;


#endif