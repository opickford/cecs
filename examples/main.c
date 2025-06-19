#include "cecs/ecs.h"

#include <string.h>
#include <Windows.h> // Sleep


// TODO: Get a better example working, how can we define components.. - this cannot be done at compile time i believe.

/*

TODO:

What i think we need is to define archetypes, essentially this is an entity with exactly the components of the archetype.
the archetype then has the arrays of components where the arrays line up to make entities

the ecs will store an array of archetypes, where the index is literally just the int? think about this, because if we have
8 components, we would have 255 archetypes... This isn't really ideal. Instead i think we could define an archetype to index
map, so if we create a new archetype, we add it to the list. This would mean we don't have hundreds of useless archetypes
and shouldn't be too difficult to manage.

Also, lets say we have multiple systems that use a 'Transform' component. But one system requires 'Transform' and 'Tag' one just
requires Transform, this means the system that just requires Transform now wants entities from multiple archetypes. So it will
have to iterate through the multiple archetypes. Would be nice to abstract this into some 'view'.

TODO:

- Allow user to define components. 
- Define an archetype, this contains an array of arrays of components.
- Manage lists of components in archetypes





*/


void log_archetypes(ECS* ecs)
{
    for (int i = 0; i < ecs->num_archetypes; ++i)
    {
        const Archetype* archetype = &ecs->archetypes[i];
        printf("Archetype\n");
        printf("bitset: %d\n" , archetype->signature.bitset);
        printf("num_components %d\n", archetype->signature.num_components);
        printf("num_entites %d\n", archetype->entity_count);

        printf("\n");
    }
    printf("-----------\n\n");
}


typedef struct
{
    float x, y, z;
} Position;

typedef struct
{
    float vx, vy, vz;
} Velocity;

int main()
{
    ECS ecs;
    ECS_init(&ecs);

    ComponentID position_component = ECS_register_component(&ecs, sizeof(Position));
    ComponentID velocity_component = ECS_register_component(&ecs, sizeof(Velocity));

    EntityID e0 = ECS_create_entity(&ecs);

    ECS_add_component(&ecs, e0, position_component);

    log_archetypes(&ecs);

    ECS_add_component(&ecs, e0, velocity_component);

    log_archetypes(&ecs);

    EntityID e1 = ECS_create_entity(&ecs);
    ECS_add_component(&ecs, e1, velocity_component);

    // TODO: Some debug stuff.
    log_archetypes(&ecs);

	return 0;
}
