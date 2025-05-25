#include "ecs.h"
#include "system.h"
#include "components.h"

#include <string.h>
#include <Windows.h> // Sleep

// TODO: System should be a part of ECS?
void test_system_update(ECS* ecs, System* system)
{
    for (int i = 0; i < system->num_entities; ++i)
    {
        const EntityID e = system->entities[i];

        Position* pos = ECS_get_Position(ecs, e);
        const Tag* tag = ECS_get_Tag(ecs, e);

        printf("%f %f %f\n", pos->x, pos->y, pos->z);
        printf("%s\n", tag->tag);

        pos->x += 1.f;
    }
    printf("\n");
}

// TODO: Write some tests for this and restrucutre everything.

int main()
{
    ECS ecs;
    ECS_init(&ecs);

    // TODO: This pointer obviously going invalid, is there a way to do this at compile time? IDK.
    // TODO: Or we just return an index 'SystemID'.
    SystemID test_system_id = ECS_add_System(&ecs);
    ecs.systems[test_system_id].signature = COMPONENT_SIGNATURE_Position | COMPONENT_SIGNATURE_Tag;

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
   
    int n = 0;

    // All systems added so pointers valid.
    System* test_system = &ecs.systems[test_system_id];

    while (1)
    {
        if (n == 5)
        {
            Position* p = ECS_add_Position(&ecs, e1);
            p->x = -10;
            p->y = -10;
            p->z = -10;
            strcpy_s(ECS_add_Tag(&ecs, e1)->tag, MAX_TAG, "WHAT THE SIGMA!");
        }
        if (n == 10)
        {
            ECS_remove_Position(&ecs, e1);
        }
        if (n == 15)
        {
            ECS_remove_entity(&ecs, e0);
        }
        if (n == 20)
        {
            e0 = ECS_create_entity(&ecs);
            ECS_add_Position(&ecs, e0);
            ECS_add_Tag(&ecs, e0);
            
        }
       
        test_system_update(&ecs, test_system);
        
        Sleep(1000);

        ++n;
    }

	return 0;
}
