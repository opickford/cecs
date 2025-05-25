#ifndef ECS_H
#define ECS_H

#include "entity.h"
#include "components.h"
#include "component_list.h"
#include "system.h"


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



      Manaing system entities:

      on component add, we must iterate through system signatures and see if the entity's signature matches, 
      



*/

typedef struct
{
    EntityID* entities;
    EntityID* free_entities;
    int count;
    int capacity;

    ComponentsSignature* signatures;

    // Component lists
    // TODO: Array of ComponentList where eaech index is Component ID? (although that's 1,2,4,8...) so would need to map those. bitshift? Do we want this?
#define X(ComponentT, i) ComponentList ComponentT##s;
    COMPONENTS_LIST
#undef X

    System* systems;
    int num_systems;

} ECS;

void ECS_init(ECS* ecs);
SystemID ECS_add_System(ECS* ecs);

// TODO: When should we capitalise???????
EntityID ECS_create_entity(ECS* ecs);
void ECS_on_add_component(ECS* ecs, EntityID id, ComponentsSignature component_signature);
void ECS_on_remove_component(ECS* ecs, EntityID id, ComponentsSignature component_signature);

// TODO: The ECS or something should manage the system signatures here.

/*
#define ECS_remove_component(ecs, entity_id, ComponentType) \
do {

} while(0)*/



/* Registering a component means to create helper functions for working with the different
   component types. Functions it defines:

   - ECS_get_ComponentType : Get's the component for the given entity.
   - ECS_add_ComponentType : Adds a component to an entity, updates any system arrays of entities if 
                             new signature matches system signature. If the entity already has the 
                             component, return the existing component.



    need to do on entity add component
    for system in systems
        if entity sig == system sig 
            add entity id to systems entites

    so how do we register a system? Can we create an array of the systems?

*/
#define ECS_register_component(ComponentType) \
inline ComponentType* ECS_get_##ComponentType(ECS* ecs, EntityID id)    \
{                                                                       \
    ComponentList* cl = &ecs->##ComponentType##s;                       \
                                                                        \
    const int i = cl->id_to_index[id];                                  \
    if (i == -1) return 0;                                              \
    return &((ComponentType*)cl->data)[i];                              \
}                                                                       \
inline ComponentType* ECS_add_##ComponentType(ECS* ecs, EntityID id)    \
{                                                                       \
    ComponentsSignature sig = ecs->signatures[id];                      \
    if (sig & COMPONENT_SIGNATURE_##ComponentType)                      \
        return ECS_get_##ComponentType(ecs, id);                        \
    ComponentList* cl = &(ecs->##ComponentType##s);                     \
    ComponentList_set_data(ComponentType, cl, id);                      \
    ecs->signatures[id] |= COMPONENT_SIGNATURE_##ComponentType;         \
    ECS_on_add_component(ecs, id, COMPONENT_SIGNATURE_##ComponentType); \
    return ECS_get_##ComponentType(ecs, id);                            \
}                                                                       \
inline void ECS_remove_##ComponentType(ECS* ecs, EntityID id)           \
{                                                                       \
    ComponentsSignature sig = ecs->signatures[id];                      \
    if (!(sig & COMPONENT_SIGNATURE_##ComponentType))                   \
        return;                                                         \
    ComponentList* cl = &(ecs->##ComponentType##s);                     \
    ComponentList_remove(ComponentType, cl, id);                        \
    ecs->signatures[id] &= ~COMPONENT_SIGNATURE_##ComponentType;        \
    ECS_on_remove_component(ecs, id, COMPONENT_SIGNATURE_##ComponentType); \
}



// Register all components.
#define X(ComponentT, _) ECS_register_component(ComponentT)
COMPONENTS_LIST
#undef X

// This shouldn't be called anywhere else so undefine.
#undef ECS_register_component





#endif