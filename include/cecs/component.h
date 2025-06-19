#ifndef COMPONENT_H
#define COMPONENT_H

// Represents a bitset of components.
typedef uint16_t ComponentsBitset; // TODO: Just signature?
typedef uint8_t ComponentID;

#define COMPONENT_BITSET_SIZE 16 // TODO: Rename to MAX COMPONENTS?
#define COMPONENTS_EMPTY_BITSET 0

#define COMPONENT_ID_TO_BITSET(id) (1 << id)

// TODO: Document.......... why all this......

// TODO: Should ID go in here?
typedef struct
{
    ComponentID id; // TODO: Pretty sure we don't want this? Not sure.
    uint32_t size;
} ComponentInfo;

typedef struct
{
    ComponentsBitset bitset;

    int num_components;
    ComponentInfo* infos;

} ComponentsSignature;


#endif