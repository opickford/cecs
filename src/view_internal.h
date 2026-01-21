#ifndef CECS_VIEW_INTERNAL_H
#define CECS_VIEW_INTERNAL_H

#include "view.h"

#include "component.h"

#include <chds/vec.h>

// TODO: should this not be like typedef struct cecs_view {} cecs_view?
typedef struct cecs_view
{
    CHDS_VEC(cecs_archetype_id) archetype_ids;

    // TODO: Document
    cecs_components_bitset include;
    cecs_components_bitset exclude;

} cecs_view;

void cecs_view_destroy(cecs_view* view);

#endif