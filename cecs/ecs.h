#ifndef ECS_H
#define ECS_H

#include "entity.h"
#include "components.h"
#include "component_list.h"



/*
Should define sparse sets for component lists,


TODO: How can we define systems? That's what we really need.
      Systems need multiple components, so we could define a group of components,
      when we add/remove a component, if an entity has components that match what
      a system needs (a group) add it to the list of entity ids for that 
      system?
      
      i think this is an okay system:
      - how do we define groups?

      look here: https://ajmmertens.medium.com/building-an-ecs-1-where-are-my-entities-and-components-63d07c7da742

      need to define a system, that defines requirements...

      system iterates component sets? like render system iterates transform and
      mesh instance

      how do we define a map for it..... how do we keep the components together.
      do we keep the entity ids or comps.



*/

typedef struct
{
    EntityID* entities;
    EntityID* free_entities;
    int count;
    int capacity;

    // Components
    // TODO: Array of ComponentList where eaech index is Component ID?
    ComponentList Positions;
    ComponentList Healths;
    ComponentList Tags;

} ECS;

void ECS_init(ECS* ecs);

EntityID ECS_create_entity(ECS* ecs);

// TODO: Doesn't even really need ecs here, could be an entity think but feels a bit odd there.
#define ECS_add_component(ecs, entity_id, ComponentType)  \
do {                                                      \
    ComponentList* cl = &((ecs)->##ComponentType##s);     \
    ComponentList_set_data(ComponentType, cl, entity_id); \
} while(0)


// TODO: What's nicer? These defines or just functions.


#define ECS_register_component(ComponentType) \
inline ComponentType* ECS_get_##ComponentType(ECS* ecs, EntityID id) \
{                                                                    \
    ComponentList* cl = &ecs->##ComponentType##s;                    \
                                                                     \
    int i = cl->id_to_index[id];                                     \
    if (i == -1) return 0;                                           \
    return &((ComponentType*)cl->data)[i];                           \
}


// Register all components.
#define X(ComponentT, _) ECS_register_component(ComponentT)
COMPONENTS_LIST
#undef X

#endif