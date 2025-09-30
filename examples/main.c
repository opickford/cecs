#include "cecs/ecs.h"

#include <string.h>
#include <Windows.h> // Sleep

#include <stdio.h>


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



// TODO: Refactor projects here like the range one, much nicer there!!!!


// TODO: How do we fix this.
ComponentID position_component;
ComponentID velocity_component;
ComponentID health_component;

typedef struct
{
    float x, y, z;
} Position;

typedef struct
{
    float vx, vy, vz;
} Velocity;

typedef struct
{
    int current;
    int max;
} Health;

void test_system_func(ECS* ecs, System* system)
{
    // TODO: make an iterator for this, would be significantly nicer.
    // for (Archetype* archetype = &ecs->archetypes[system->archetype_ids[0]; )

    for (int si = 0; si < system->num_archetypes; ++si)
    {
        ArchetypeID archetype_id = system->archetype_ids[si];
        Archetype* archetype = &ecs->archetypes[archetype_id];

        Position* positions = Archetype_get_component_list(archetype, position_component);
        Velocity* velocities = Archetype_get_component_list(archetype, velocity_component);

        for (int i = 0; i < archetype->entity_count; ++i)
        {
            Position p = positions[i];
            Velocity v = velocities[i];

            printf("Position: %f %f %f\n", p.x, p.y, p.z);
            printf("Velocity: %f %f %f\n", v.vx, v.vy, v.vz);
        }

        printf("End of archetype\n");
    }
}


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

int main()
{
    ECS ecs;
    ECS_init(&ecs);

    position_component = ECS_register_component(&ecs, sizeof(Position));
    velocity_component = ECS_register_component(&ecs, sizeof(Velocity));
    health_component = ECS_register_component(&ecs, sizeof(Health));


    // Register a test system that uses both components.
    // TODO: Make nicer somehow idk?
    SystemID test_system_id = ECS_register_system(&ecs);
    System* test_system = &ecs.systems[test_system_id];

    // TODO: Define for this?
    test_system->components_bitset =
        COMPONENT_ID_TO_BITSET(position_component) | 
        COMPONENT_ID_TO_BITSET(velocity_component);


    int num_entities = 100000;
    
    // TODO: Document how component pointers are only valid until the 
    //       next component is added!! or like how it actually works....
    
    EntityID e0 = ECS_create_entity(&ecs);

    Position* e0_pos = ECS_add_component(&ecs, e0, position_component);
    e0_pos->x = 1;
    e0_pos->y = 1;
    e0_pos->z = 1;

    Velocity* e0_vel = ECS_add_component(&ecs, e0, velocity_component);
    e0_vel->vx = -1;
    e0_vel->vy = -1;
    e0_vel->vz = -1;

    
    ECS_remove_component(&ecs, e0, velocity_component);
    
    EntityID e1 = ECS_create_entity(&ecs);
    ECS_add_component(&ecs, e1, velocity_component);
    ECS_add_component(&ecs, e1, position_component);
    ECS_add_component(&ecs, e1, health_component);

    ECS_remove_component(&ecs, e1, health_component);
    ECS_remove_component(&ecs, e1, velocity_component);
    ECS_remove_component(&ecs, e1, position_component);

    ECS_destroy_entity(&ecs, e1);
    
    e0_vel = ECS_add_component(&ecs, e0, velocity_component);
    *e0_vel = (Velocity){ 1,2,3 };

    // TODO: Make a func pointer in the system and let ecs tick.
    test_system_func(&ecs, test_system);
    
	return 0;
}
