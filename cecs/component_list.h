#ifndef COMPONENT_LIST_H
#define COMPONENT_LIST_H

#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    void* data;
    int count;
    int capacity;

    int* id_to_index;

} ComponentList;

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
    (component_list)->id_to_index[id] = (component_list)->count;             \
    (component_list)->count++;                                               \
} while (0)

#endif