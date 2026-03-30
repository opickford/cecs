#ifndef CECS_VIEW_INTERNAL_H
#define CECS_VIEW_INTERNAL_H

#include "view.h"

#include "component.h"

#include <chds/vec.h>

// TODO: should this not be like typedef struct CECS_View {} CECS_View?
typedef struct CECS_View
{
    CHDS_Vec(CECS_ArchetypeId) archetype_ids;

    // TODO: Document
    CECS_ComponentsBitset include;
    CECS_ComponentsBitset exclude;

} CECS_View;

void cecs_view_destroy(CECS_View* view);

#endif