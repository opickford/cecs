#include "view_internal.h"

void cecs_view_destroy(CECS_View* view)
{
    // TODO: Some cecs_destroy_view should probs call this.
    chds_vec_destroy(view->archetype_ids);
}