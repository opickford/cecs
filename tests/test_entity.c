#ifndef TEST_ENTITY_H
#define TEST_ENTITY_H

#include <cecs/ecs.h>

#include <assert.h>

static void test_create()
{
    cecs* ecs = cecs_create();
    cecs_entity_id e = cecs_create_entity(ecs);

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