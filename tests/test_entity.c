#ifndef TEST_ENTITY_H
#define TEST_ENTITY_H

#include <CECS/ecs.h>

#include <assert.h>

static void test_create()
{
    CECS* ecs = cecs_create();
    CECS_EntityId e = cecs_create_entity(ecs);

    assert(e != INVALID_ENTITY);
}

static void test_get_component_invalid_entity()
{
    CECS* ecs = cecs_create();
    CECS_EntityId e = cecs_create_entity(ecs);
    cecs_destroy_entity(ecs, e);

    // TODO: note this is also a invalid component id.
    void* comp = cecs_get_component(ecs, e, 0);
    assert(comp == 0);
}




static void test_entity()
{
    test_create();
    test_get_component_invalid_entity();
}

int main()
{
    test_entity();
    return 0;
}

#endif