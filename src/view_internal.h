#ifndef VIEW_INTERNAL_H
#define VIEW_INTERNAL_H

#include "view.h"

#include "component.h"

#include <chds/vec.h>

typedef struct View
{
    chds_vec(ArchetypeID) archetype_ids;

    // TODO: Document
    ComponentsBitset include;
    ComponentsBitset exclude;

} View;

void View_destroy(View* view);

#endif