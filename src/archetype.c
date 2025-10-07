#include "archetype_internal.h"

#include <stdlib.h>
#include <string.h>

// TODO: Not really necessary.
void cecs_archetype_init(cecs_archetype_t* archetype)
{
    memset(archetype, 0, sizeof(cecs_archetype_t));
}

void cecs_archetype_destroy(cecs_archetype_t* archetype)
{
    for (int i = 0; i < archetype->signature.num_components; ++i)
    {
        free(archetype->columns[i]);
    }

    chds_vec_destroy(archetype->index_to_entity);

    archetype = 0;
}

// TODO: Currently a linear search, not ideal. Low number of components should be 
//       fine but really want to improve this.
// TODO: This sort of thing could be cached right? DEFINITELY. maybe this could relate to doing 'views'.
void* cecs_archetype_get_column(cecs_archetype_t* archetype, cecs_component_id_t cid)
{
    const cecs_components_signature_t* signature = &archetype->signature;
    for (int i = 0; i < signature->num_components; ++i)
    {
        if (signature->infos[i].id == cid)
        {
            return archetype->columns[i];
        }
    }

    // TODO: How else.
    return 0;
}