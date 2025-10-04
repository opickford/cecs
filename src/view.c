#include "view_internal.h"

void View_destroy(View* view)
{
    Vector_destroy(view->archetype_ids);
}