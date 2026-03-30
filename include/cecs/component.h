#ifndef CECS_COMPONENT_H
#define CECS_COMPONENT_H

#include <stdint.h>

// TODO: What is private here and what isn't.

/*

TODO: Probably gotta rethink all of this at some point.
// TODO: Write this out better anyways, don't need docs at top just above
         types etc.

CECS_ComponentId
- The CECS stores an array of components, hence, CECS_ComponentId is an index into that array.
- TODO: Static sized array of components in ecs?

ComponentBitset:
- Represents flags for present components, this is for quicker comparisons.

CECS_ComponentInfo: 
- Stores the sizeof(ComponentT) and it's ID (index in ecs components array).

CECS_ComponentsSignature:
- Stores a bitset and array of component infos.

*/

// Represents a bitset of components.
// TODO: Issue this only allows for 16 components. 
//       In the future we could probably expand this to some array of bitsets.
//       but that should be in the CHDS lib?
typedef uint16_t CECS_ComponentsBitset; // TODO: Just signature?
typedef uint8_t CECS_ComponentId;

#define CECS_MAX_COMPONENTS 16
#define CECS_EMPTY_COMPONENTS_BITSET 0

#define CECS_COMPONENT_ID_TO_BITSET(id) (CECS_ComponentsBitset)(1 << id)

#endif