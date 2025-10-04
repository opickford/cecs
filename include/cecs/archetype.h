#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include <stdint.h>

typedef uint16_t ArchetypeID;
#define INVALID_ARCHETYPE UINT16_MAX

// TODO: How do we hide the Vector from the user?????
//       It's meaning that my engine cannot build as it cannot find
//       chds/vector.h which doesn't make sense... right?
// TODO: I think the solution is to not define Archetype in the header, 
//       it can just be used internally? maybe??? but then how do we get
//       the component lists????

typedef struct Archetype Archetype;

size_t Archetype_num_entities(const Archetype* archetype);

#endif