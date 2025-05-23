#ifndef COMPONENTS_H
#define COMPONENTS_H

// Represents a bitset of components.
typedef uint16_t ComponentsSignature;

// Define example components.
typedef struct
{
    float x, y, z;
} Position;

typedef struct
{
    int current;
    int max;
} Health;

#define MAX_TAG 64
typedef struct
{
    char tag[MAX_TAG];
} Tag;

// Define component signatures.

#define COMPONENTS_LIST \
    X(Position, 0)      \
    X(Health, 1)        \
    X(Tag, 2)

enum {
#define X(ComponentT, i) COMPONENT_##ComponentT = (1 << i),
    COMPONENTS_LIST
#undef X
};

#endif