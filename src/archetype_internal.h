#ifndef ARCHETYPE_INTERNAL_H
#define ARCHETYPE_INTERNAL_H

#include "archetype.h"

#include "entity.h"
#include "component.h"

#include <chds/vector.h>

typedef struct Archetype
{
    ComponentsSignature signature;

    // TODO: Some sort of map?
    Vector(EntityID) index_to_entity;

    void** component_lists;

} Archetype;

// TODO: A bit misleading as the ECS actually properly initialises this.
void Archetype_init(Archetype* archetype);
void Archetype_destroy(Archetype* archetype);
void* Archetype_get_component_list(Archetype* archetype, ComponentID cid);

#endif