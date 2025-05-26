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




// TODO: System should be a part of ECS?
void test_system_update(ECS* ecs, System* system)
{
    // TODO: I've realised this pretty much entirely defeats the point of the ECS.
    //       we're no longer itertating through tightly packed component lists.
    /*
    
    basically desired code:

    for i in num_entities_with_archetype
        render(transforms[i], mesh_instances[i])


    */

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
            Position* p = ECS_add_Position(&ecs, e0);
            p->x = 0;
            p->y = 0;
            p->z = 0;
            strcpy_s(ECS_add_Tag(&ecs, e0)->tag, MAX_TAG, "NEW!");
        }
       
        test_system_update(&ecs, test_system);
        
        Sleep(1000);

        ++n;
    }

	return 0;
}
