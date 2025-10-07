#ifndef CECS_VIEW_INTERNAL_H
#define CECS_VIEW_INTERNAL_H

#include "view.h"

#include "component.h"

#include <chds/vec.h>

typedef struct cecs_view_t
{
    chds_vec(cecs_archetype_id_t) archetype_ids;

    // TODO: Document
    cecs_components_bitset_t include;
    cecs_components_bitset_t exclude;

} cecs_view_t;

void cecs_view_destroy(cecs_view_t* view);

#endif