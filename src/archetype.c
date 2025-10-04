#include "archetype_internal.h"

#include <stdlib.h>
#include <string.h>

// TODO: Not really necessary.
void Archetype_init(Archetype* archetype)
{
    memset(archetype, 0, sizeof(Archetype));
}

void Archetype_destroy(Archetype* archetype)
{
    for (int i = 0; i < archetype->signature.num_components; ++i)
    {
        free(archetype->component_lists[i]);
    }

    Vector_destroy(archetype->index_to_entity);

    archetype = 0;
}

size_t Archetype_num_entities(const Archetype* archetype)
{
    return Vector_size(archetype->index_to_entity);
}

// TODO: Currently a linear search, not ideal. Low number of components should be 
//       fine but really want to improve this.
// TODO: This sort of thing could be cached right? DEFINITELY. maybe this could relate to doing 'views'.
void* Archetype_get_component_list(Archetype* archetype, ComponentID cid)
{
    const ComponentsSignature* signature = &archetype->signature;
    for (int i = 0; i < signature->num_components; ++i)
    {
        if (signature->infos[i].id == cid)
        {
            return archetype->component_lists[i];
        }
    }

    // TODO: How else.
    return 0;
}