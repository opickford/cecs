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

static void test_entity()
{
    test_create();
}

int main()
{
    test_entity();
    return 0;
}

#endif