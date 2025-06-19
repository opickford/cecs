#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include "component.h"
#include "entity.h"

#include <string.h>
#include <stdlib.h>

typedef uint16_t ArchetypeID;
#define INVALID_ARCHETYPE UINT16_MAX

typedef struct
{
    ComponentsSignature signature;
    
    int entity_count;
    int entity_capacity;

    EntityID* index_to_entity;
    
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
inline int Archetype_find_component_list(Archetype* archetype, ComponentID cid)
{
    const ComponentsSignature* signature = &archetype->signature;
    for (int i = 0; i < signature->num_components; ++i)
    {
        if (signature->infos[i].id == cid)
        {
            return i;
        }
    }

    // TODO: How else.
    return -1;
}







#endif