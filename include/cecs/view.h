#ifndef SYSTEM_H
#define SYSTEM_H

#include "archetype.h"
#include "component.h"

#include <string.h>

/*
TODO: Comments.

View must store the archetypes it cares about, the ecs can manage this for us.

*/

typedef uint8_t ViewID;

typedef struct
{
    ArchetypeID* archetype_ids;
    int num_archetypes;

    ComponentsBitset components_bitset;

} View;

inline void View_init(View* view)
{
    memset(view, 0, sizeof(View));
}

inline void View_add_archetype(View* view, ArchetypeID archetype_id)
{
    // TODO: again, all of this stuff could use some vector implementation
    //       or just grow array by a factor to avoid allocs.

    ArchetypeID* temp_archetype_ids = realloc(view->archetype_ids,
        (view->num_archetypes + 1) * sizeof(ArchetypeID));

    if (!temp_archetype_ids)
    {
        // TODO: Handle failure.
        return;
    }

    view->archetype_ids = temp_archetype_ids;

    view->archetype_ids[view->num_archetypes] = archetype_id;
    ++view->num_archetypes;
}

inline void View_destroy(View* view)
{
    free(view->archetype_ids);
}

#endif