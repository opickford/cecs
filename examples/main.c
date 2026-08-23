#include "CECS/ecs.h"

#include <string.h>
#include <Windows.h> // Sleep

#include <stdio.h>

// TODO: Get a better example working, how can we define components.. - this cannot be done at compile time i believe.

// TODO: How do we fix this.
CECS_ComponentId position_component;
CECS_ComponentId velocity_component;
CECS_ComponentId health_component;

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

static void test_func(CECS* ecs, CECS_ViewId view_id)
{
    CECS_ViewIter it = cecs_view_iter_create(ecs, view_id);
    while (cecs_view_iter_next(&it))
    {
        const CECS_EntityId* ids = cecs_get_entity_ids(it);
        Position* positions = cecs_get_column(it, position_component);
        Velocity* velocities = cecs_get_column(it, velocity_component);

        for (uint32_t i = 0; i < it.num_entities; ++i)
        {
            CECS_EntityId id = ids[i];
            Position p = positions[i];
            Velocity v = velocities[i];

            printf("Entity %u\n", id);
            printf("Position: %f %f %f\n", p.x, p.y, p.z);
            printf("Velocity: %f %f %f\n", v.vx, v.vy, v.vz);
        }

        printf("End of archetype\n");
    }
}

int main()
{
    CECS* ecs = cecs_create();
    
    position_component = cecs_register_component(ecs, sizeof(Position));
    velocity_component = cecs_register_component(ecs, sizeof(Velocity));
    health_component = cecs_register_component(ecs, sizeof(Health));

    // Register a test view that uses position, velocity but excludes health.
    CECS_ViewId test_view_id = cecs_view_create(ecs,
        CECS_COMPONENT_ID_TO_BITSET(position_component) | CECS_COMPONENT_ID_TO_BITSET(velocity_component),
        CECS_COMPONENT_ID_TO_BITSET(health_component)
    );

    // TODO: Document how component pointers are only valid until the 
    //       next component is added!! or like how it actually works....
    CECS_EntityId e0 = cecs_create_entity(ecs);

    Position* e0_pos = cecs_add_component(ecs, e0, position_component);
    e0_pos->x = 1;
    e0_pos->y = 1;
    e0_pos->z = 1;

    Velocity* e0_vel = cecs_add_component(ecs, e0, velocity_component);
    e0_vel->vx = -1;
    e0_vel->vy = -1;
    e0_vel->vz = -1;

    cecs_remove_component(ecs, e0, velocity_component);
    
    // TODO: Can definitely rename to -> cecs_entity instead,
    //       the id part is unnecessary.
    CECS_EntityId e1 = cecs_create_entity(ecs);
    cecs_add_component(ecs, e1, velocity_component);
    cecs_add_component(ecs, e1, position_component);
    cecs_add_component(ecs, e1, health_component);

    cecs_remove_component(ecs, e1, health_component);
    cecs_remove_component(ecs, e1, velocity_component);
    cecs_remove_component(ecs, e1, position_component);

    cecs_destroy_entity(ecs, e1);
    
    e0_vel = cecs_add_component(ecs, e0, velocity_component);
    *e0_vel = (Velocity){ 1,2,3 };

    test_func(ecs, test_view_id);
    
	return 0;
}
