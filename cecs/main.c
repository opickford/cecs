// ecs.cpp : Defines the entry point for the application.
//

#include "ecs.h"
#include "system.h"
#include "components.h"

#include <string.h>
#include <Windows.h> // sleep

// TODO: If this all works great could turn into separate github project.

// TODO: System should be a part of entity?
void test_system_update(ECS* ecs, System* system)
{
    for (int i = 0; i < system->num_entities; ++i)
    {
        EntityID e = system->entities[i];

        Position* pos = ECS_get_Position(ecs, e);
        Tag* tag = ECS_get_Tag(ecs, e);

        printf("%f %f %f\n", pos->x, pos->y, pos->z);
        printf("%s\n", tag->tag);

        pos->x += 1.f;
    }
}

int main()
{
    ECS ecs;
    ECS_init(&ecs);

    System test_system;
    System_init(&test_system);
    test_system.signature = COMPONENT_SIGNATURE_Position | COMPONENT_SIGNATURE_Tag;

    EntityID e0 = ECS_create_entity(&ecs);
    EntityID e1 = ECS_create_entity(&ecs);

    Position* pos = ECS_add_Position(&ecs, e0);

    pos->x = 5.f;
    pos->y = 5.f;
    pos->z = 5.f;
    
    Tag* tag = ECS_add_Tag(&ecs, e0);
    strcpy_s(tag->tag, MAX_TAG, "abc");

    printf("%d\n", e0);
    printf("%d\n", e1);

    printf("%d\n", ecs.signatures[e0]);
    printf("%d\n", ecs.signatures[e1]);
    /*
    while (1)
    {
        test_system_update(&ecs, &test_system);
        
        Sleep(1000);
    }*/

	return 0;
}
