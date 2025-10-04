#include "view_internal.h"

void View_destroy(View* view)
{
    // TODO: Some ECS_destroy_view should probs call this.
    Vector_destroy(view->archetype_ids);
}