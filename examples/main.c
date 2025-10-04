#include "cecs/ecs.h"

#include <string.h>
#include <Windows.h> // Sleep

#include <stdio.h>

// TODO: Get a better example working, how can we define components.. - this cannot be done at compile time i believe.

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

static void test_func(ECS* ecs, ViewID view_id)
{
    // TODO: I don't really like the name of these functions.
    ViewIter it = ECS_view_iter(ecs, view_id);
    while (ECS_view_iter_next(&it))
    {
        Position* positions = ECS_get_component_list(it, position_component);
        Velocity* velocities = ECS_get_component_list(it, velocity_component);

        for (uint32_t i = 0; i < it.num_entities; ++i)
        {
            Position p = positions[i];
            Velocity v = velocities[i];

            printf("Position: %f %f %f\n", p.x, p.y, p.z);
            printf("Velocity: %f %f %f\n", v.vx, v.vy, v.vz);
        }

        printf("End of archetype\n");
    }
}

int main()
{
    ECS* ecs = ECS_create();
    
    position_component = ECS_register_component(ecs, sizeof(Position));
    velocity_component = ECS_register_component(ecs, sizeof(Velocity));
    health_component = ECS_register_component(ecs, sizeof(Health));

    // TODO: Document how component pointers are only valid until the 
    //       next component is added!! or like how it actually works....
    EntityID e0 = ECS_create_entity(ecs);

    Position* e0_pos = ECS_add_component(ecs, e0, position_component);
    e0_pos->x = 1;
    e0_pos->y = 1;
    e0_pos->z = 1;

    Velocity* e0_vel = ECS_add_component(ecs, e0, velocity_component);
    e0_vel->vx = -1;
    e0_vel->vy = -1;
    e0_vel->vz = -1;

    ECS_remove_component(ecs, e0, velocity_component);
    
    EntityID e1 = ECS_create_entity(ecs);
    ECS_add_component(ecs, e1, velocity_component);
    ECS_add_component(ecs, e1, position_component);
    ECS_add_component(ecs, e1, health_component);

    ECS_remove_component(ecs, e1, health_component);
    ECS_remove_component(ecs, e1, velocity_component);
    ECS_remove_component(ecs, e1, position_component);

    ECS_destroy_entity(ecs, e1);
    
    e0_vel = ECS_add_component(ecs, e0, velocity_component);
    *e0_vel = (Velocity){ 1,2,3 };

    // Register a test view that uses position, velocity but excludes health.
    ViewID test_view_id = ECS_view(ecs,
        COMPONENT_ID_TO_BITSET(position_component) | COMPONENT_ID_TO_BITSET(velocity_component),
        COMPONENT_ID_TO_BITSET(health_component)
    );
    test_func(ecs, test_view_id);
    
	return 0;
}
