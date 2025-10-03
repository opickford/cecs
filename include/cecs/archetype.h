#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include "component.h"
#include "entity.h"

#include <chds/vector.h>

#include <string.h>
#include <stdlib.h>

typedef uint16_t ArchetypeID;
#define INVALID_ARCHETYPE UINT16_MAX

typedef struct
{
    ComponentsSignature signature;
    
    // TODO: Some sort of map?
    Vector(EntityID) index_to_entity;
    
    void** component_lists;

} Archetype;

// TODO: A bit misleading as the ECS actually properly initialises this.
inline void Archetype_init(Archetype* archetype)
{
    memset(archetype, 0, sizeof(Archetype));
}

inline void Archetype_destroy(Archetype* archetype)
{
    for (int i = 0; i < archetype->signature.num_components; ++i)
    {
        free(archetype->component_lists[i]);
    }

    free(archetype->index_to_entity);

    archetype = 0;
}

// TODO: Currently a linear search, not ideal. Low number of components should be 
//       fine but really want to improve this.
// TODO: This sort of thing could be cached right? DEFINITELY. maybe this could relate to doing 'views'.
inline void* Archetype_get_component_list(Archetype* archetype, ComponentID cid)
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







#endif