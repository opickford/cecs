// ecs.cpp : Defines the entry point for the application.
//

#include "ecs.h"
#include "system.h"
#include "components.h"

#include <string.h>

// TODO: If this all works great could turn into separate github project.

int main()
{
    
    ECS ecs;
    ECS_init(&ecs);

    System test_system;
    System_init(&test_system);
    test_system.signature = COMPONENT_Position | COMPONENT_Tag;

    EntityID e0 = ECS_create_entity(&ecs);
    EntityID e1 = ECS_create_entity(&ecs);

    ECS_add_component(&ecs, e0, Position);
    ECS_add_component(&ecs, e0, Tag);

    Position* pos = ECS_get_Position(&ecs, e0);
    Position p = *pos;
    printf("%f %f %f\n", p.x, p.y, p.z);

    Tag* tag = ECS_get_Tag(&ecs, e0);
    strcpy_s(tag->tag, MAX_TAG, "abc");

    printf("%s\n", tag->tag);

	return 0;
}
