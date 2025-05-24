#ifndef COMPONENTS_H
#define COMPONENTS_H

// Represents a bitset of components.
typedef uint16_t ComponentsSignature; // TODO: Just signature?
#define EMPTY_SIGNATURE 0

// Define example components - these can be defined in separate headers if preferred.
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

// All components must be added to this list.
#define COMPONENTS_LIST \
    X(Position, 0)      \
    X(Health, 1)        \
    X(Tag, 2)

// Define bit signatures per component and index per component.
enum {
#define X(ComponentT, i) COMPONENT_SIGNATURE_##ComponentT = (1 << i),
    COMPONENTS_LIST
#undef X
};

enum {
#define X(ComponentT, i) COMPONENT_INDEX_##ComponentT = i,
    COMPONENTS_LIST
#undef X
    COMPONENTS_COUNT
};

#endif