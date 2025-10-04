#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"
#include "component.h"

#include <chds/vector.h>

#include <string.h>

typedef uint8_t ViewID;

// TODO: Some function for passing in component ids separately.

/* A view holds ptrs to the archetypes matching it's component bitset. 
   The ECS manages adding the archetypes to the view.
*/ 
typedef struct
{
    Vector(ArchetypeID) archetype_ids;

    // TODO: Document
    ComponentsBitset include;
    ComponentsBitset exclude;

} View;

// TODO: Needs refactor.
// TODO: Can contain pointer to ecs.

typedef struct ECS ECS;
typedef struct
{
    const ECS* ecs;

    ViewID vid;

    int current;
    int end;

    ArchetypeID* aid;

    uint32_t count;

} ViewIter;

/* TODO: Not used. Do we need it?
inline void View_init(View* view)
{
    memset(view, 0, sizeof(View));

    // TODO: Should take in a components_bitset? 
}*/



// TODO: Private, user should not call this.
inline void View_destroy(View* view)
{
    Vector_destroy(view->archetype_ids);
}

#endif