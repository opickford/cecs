#ifndef COMPONENT_LIST_H
#define COMPONENT_LIST_H

#include "entity.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    void* data;
    int count;
    int capacity;

    int* id_to_index;
    EntityID* index_to_id;

} ComponentList;

// TODO: The name of this is misleading.
#define ComponentList_set_data(ComponentType, component_list, id)            \
do {                                                                         \
    if ((component_list)->count == (component_list)->capacity)               \
    {                                                                        \
        (component_list)->capacity++;                                        \
        ComponentType* temp = realloc((component_list)->data,                \
            (component_list)->capacity * sizeof(ComponentType));             \
        if (temp)                                                            \
        {                                                                    \
            (component_list)->data = temp;                                   \
        }                                                                    \
        else                                                                 \
        {                                                                    \
            printf("component_list_set_data failed ....\n");                 \
            break;                                                           \
        }                                                                    \
    }                                                                        \
    (component_list)->id_to_index[(id)] = (component_list)->count;           \
    (component_list)->index_to_id[(component_list)->count] = (id);           \
    (component_list)->count++;                                               \
} while (0)

/*

to remove....




*/

#define ComponentList_remove(ComponentType, component_list, id)                                        \
do                                                                                                     \
{                                                                                                      \
    const int index_to_remove = (component_list)->id_to_index[(id)];                                   \
    const EntityID last_id = (component_list)->index_to_id[(component_list)->count - 1];               \
                                                                                                       \
    /* Show entity does not have component by setting invalid index. */                                \
    (component_list)->id_to_index[(id)] = -1;                                                          \
                                                                                                       \
    /* Set the last entity to be the one we're removing, to keep the array packed. */                  \
    (component_list)->index_to_id[index_to_remove] = last_id;                                          \
                                                                                                       \
    /* TODO: Destroy old data? Incase of dynamicly allocated. Function like ComponentType_destroy */   \
                                                                                                       \
    /* Copy the data we're swapping. */                                                                \
    ComponentType* elements = (ComponentType*)(component_list)->data;                                  \
    memcpy(&elements[index_to_remove], &elements[(component_list)->count - 1], sizeof(ComponentType)); \
                                                                                                       \
} while(0)



#endif