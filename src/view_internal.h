#ifndef VIEW_INTERNAL_H
#define VIEW_INTERNAL_H

#include "view.h"

#include "component.h"

#include <chds/vector.h>

typedef struct View
{
    Vector(ArchetypeID) archetype_ids;

    // TODO: Document
    ComponentsBitset include;
    ComponentsBitset exclude;

} View;

void View_destroy(View* view);

#endif