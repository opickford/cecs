#include "ecs.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void ECS_init(ECS* ecs)
{
    memset(ecs, 0, sizeof(ECS));

    // Ensure an empty archetype exists to store entities without components.
    // NOTE: This logic may be refactored in the future so that there is no 
    //       need for this empty archetype! The issue being removing an entity's
    //       last component. Also, keeping this simplifies other logic as it means
    //       an entity always has an archetype!
    ArchetypeID empty_archetype = ECS_create_archetype(ecs, 0);

    // Ensure the archetype is valid and matches the hardcoded index.
    if (empty_archetype == INVALID_ARCHETYPE || empty_archetype != EMPTY_ARCHETYPE_ID)
    {
        // TODO: Handle error!
        assert(0);
    }
}

ComponentID ECS_register_component(ECS* ecs, uint32_t component_size)
{
    // Store the size of the component for resizing archetype arrays.
    ComponentInfo ci = {
        .size = component_size,
        .id = Vector_size(ecs->component_infos)
    };
    Vector_push_back(ecs->component_infos, ci);

    return ci.id;
}

ViewID ECS_register_view(ECS* ecs)
{
    ViewID view_id = Vector_size(ecs->views);
    Vector_push_back(ecs->views, (View) { 0 });

    View* view = &ecs->views[view_id];
    View_init(view);

    return view_id;
}

EntityID ECS_create_entity(ECS* ecs)
{
    EntityID entity;

    ++ecs->num_used_entities;

    // Re-use if free Entity.
    if (ecs->free_entities_count > 0)
    {
        // Free entities works like a stack.
        entity = ecs->free_entities[ecs->free_entities_count - 1];

        --ecs->free_entities_count;
    }
    else
    {
        const int total_created_entities = ecs->num_used_entities + ecs->free_entities_count;

        // TODO: Grow by some factor to save allocations?
        // TODO: Really need to do this!
        ComponentsBitset* temp_bitsets = realloc(ecs->entity_components_bitsets,
            total_created_entities * sizeof(ComponentsBitset));

        if (!temp_bitsets)
        {
            printf("Failed to alloc for ecs->entity_components_bitsets.\n");
            return INVALID_ENTITY;
        }
        ecs->entity_components_bitsets = temp_bitsets;

        EntityIndex* temp_indices = realloc(ecs->entity_indices, 
            total_created_entities * sizeof(EntityIndex));
        if (!temp_indices)
        {
            printf("Failed to alloc for ecs->entity_indices.\n");
            return INVALID_ENTITY;
        }
        ecs->entity_indices = temp_indices;

        entity = total_created_entities - 1;   
    }
    
    // Initialise entity data.
    ecs->entity_components_bitsets[entity] = COMPONENTS_EMPTY_BITSET;
    ecs->entity_indices[entity].archetype_id = -1;
    ecs->entity_indices[entity].component_list_index = -1;

    // Move the entity to the empty archetype.
    ECS_move_archetype(ecs, entity, INVALID_ARCHETYPE, EMPTY_ARCHETYPE_ID);
    
    return entity;
}

// TODO: Rename destroy?
void ECS_destroy_entity(ECS* ecs, EntityID id)
{
    --ecs->num_used_entities;

    // Entities don't need to be packed as we will never be iterating over the entities loop.

    // Clear entity signature.
    const ComponentsBitset old_bitset = ecs->entity_components_bitsets[id];
    ecs->entity_components_bitsets[id] = COMPONENTS_EMPTY_BITSET;

    // Grow capacity if needed.
    if (ecs->free_entities_count == ecs->free_entities_capacity)
    {
        // TODO: Grow by factor?
        ++ecs->free_entities_capacity;

        EntityID* temp_free_entities = realloc(ecs->free_entities, ecs->free_entities_capacity * sizeof(EntityID));
        if (!temp_free_entities)
        {
            printf("failed to grow ecs->free_entities\n");
            return;
        }
        ecs->free_entities = temp_free_entities;
    }

    // Save the new free entity.
    ecs->free_entities[ecs->free_entities_count] = id;
    ++ecs->free_entities_count;

    // Remove entity from it's archetype, which should remove it's components,
    // note, sadly we have to search for the archetype. Could use a map from
    // entity id to archetype id (index). TODO: consider.
    int num_archetypes = Vector_size(ecs->archetypes);
    for (int i = 0; i < num_archetypes; ++i)
    {
        Archetype* archetype = &ecs->archetypes[i];
        if (archetype->signature.bitset == old_bitset)
        {
            Archetype_remove_entity(ecs, archetype, ecs->entity_indices[id].component_list_index);
            break;
        }
    }
}

void* ECS_add_component(ECS* ecs, EntityID eid, ComponentID cid)
{
    const ComponentsBitset old_components_bitset = ecs->entity_components_bitsets[eid];
    const ComponentID component_bitset = COMPONENT_ID_TO_BITSET(cid);

    // Entity already has component, return that component instead.
    if (old_components_bitset & component_bitset)
    {
        return ECS_get_component(ecs, eid, cid);
    }
 
    // Update entity's signature with the new component.
    ecs->entity_components_bitsets[eid] |= component_bitset;

    // TODO: If matching archetype doesn't exist, create one.
    // TODO: With this, maybe we don't want an archetype for just one component?
    //       Somehow we could delay archetype creation. (flag for it?)

    // TODO: Is there a nicer way to do this using some sort of mapping, brute 
    //       force search not ideal but shouldn't be a big issue for now.

    // Must use ArchetypeID as if we create a new archetype we will invalidate the pointers.
    ArchetypeID new_archetype_id = INVALID_ARCHETYPE;
    ArchetypeID old_archetype_id = INVALID_ARCHETYPE;
    const int num_archetypes = Vector_size(ecs->archetypes);
    for (int i = 0; i < num_archetypes; ++i)
    {
        if (ecs->archetypes[i].signature.bitset == old_components_bitset)
        {
            old_archetype_id = i;
        }
        if (ecs->archetypes[i].signature.bitset == ecs->entity_components_bitsets[eid])
        {
            new_archetype_id = i;
        }
    }

    // Create new archetype to match entity signature.
    if (new_archetype_id == INVALID_ARCHETYPE)
    {
        new_archetype_id = ECS_create_archetype(ecs, ecs->entity_components_bitsets[eid]);
    }

    ECS_move_archetype(ecs, eid, old_archetype_id, new_archetype_id);

    // Return pointer to new component.
    return ECS_get_component(ecs, eid, cid);
}

void ECS_remove_component(ECS* ecs, EntityID eid, ComponentID cid)
{
    const ComponentsBitset old_components_bitset = ecs->entity_components_bitsets[eid];
    const ComponentID component_bitset = COMPONENT_ID_TO_BITSET(cid);

    // Entity doesn't have the component.
    if (!(old_components_bitset & component_bitset))
    {
        return;
    }

    // Remove component from entity's bitset.
    ecs->entity_components_bitsets[eid] &= (~component_bitset);

    // TODO: If matching archetype doesn't exist, create one.
    // TODO: With this, maybe we don't want an archetype for just one component?
    //       Somehow we could delay archetype creation. (flag for it?)

    // TODO: Is there a nicer way to do this using some sort of mapping, brute 
    //       force search not ideal but shouldn't be a big issue for now.

    // Must use ArchetypeID as if we create a new archetype we will invalidate the pointers.
    ArchetypeID new_archetype_id = INVALID_ARCHETYPE;
    ArchetypeID old_archetype_id = INVALID_ARCHETYPE;
    const int num_archetypes = Vector_size(ecs->archetypes);
    for (int i = 0; i < num_archetypes; ++i)
    {
        if (ecs->archetypes[i].signature.bitset == old_components_bitset)
        {
            old_archetype_id = i;
        }
        if (ecs->archetypes[i].signature.bitset == ecs->entity_components_bitsets[eid])
        {
            new_archetype_id = i;
        }
    }

    // NOTE: Currently this should never happen, however, in the future if we update the 
    // logic so that we don't create a new archetype for every new components bitset,
    // then this will be necessary, so just future proofing.
    if (new_archetype_id == INVALID_ARCHETYPE)
    {
        new_archetype_id = ECS_create_archetype(ecs, ecs->entity_components_bitsets[eid]);
    }

    ECS_move_archetype(ecs, eid, old_archetype_id, new_archetype_id);
}

void* ECS_get_component(ECS* ecs, EntityID eid, ComponentID cid)
{
    EntityIndex ei = ecs->entity_indices[eid];
    Archetype* archetype = &ecs->archetypes[ei.archetype_id];
    
    int size = ecs->component_infos[cid].size;

    // Convert to uint8_t for pointer arithmetic.
    uint8_t* component_list = Archetype_get_component_list(archetype, cid);
    if (component_list == 0)
    {
        // TODO: Handle failure
        return 0;
    }
    
    void* component = component_list + ei.component_list_index * size;
    return component;
}

// Internal helper functions
ArchetypeID ECS_create_archetype(ECS* ecs, ComponentsBitset archetype_bitset)
{
    // Currently we're allowing for an empty archetype to keep the logic simple,
    // so that every entity lives in an archetype. May change in the future.
    ArchetypeID archetype_id = Vector_size(ecs->archetypes);
    Archetype new_archetype = {
        .signature = {
            .bitset = archetype_bitset
        }
    };
    Vector_push_back(ecs->archetypes, new_archetype);
    Archetype* archetype = &ecs->archetypes[archetype_id];

    // TODO: Remove Archetype_init?
    //Archetype_init(archetype);

    // Create archetype signature from bitset.
    for (int i = 0; i < COMPONENT_BITSET_SIZE; ++i)
    {
        ComponentsBitset bitset = COMPONENT_ID_TO_BITSET(i);

        // Check if the archetype should have this component.
        if (bitset & archetype_bitset)
        {
            ComponentInfo* temp = realloc(archetype->signature.infos,
                (size_t)(archetype->signature.num_components + 1) * 
                sizeof(ComponentInfo));

            if (!temp)
            {
                // TODO: Handle failure.
                return 0;
            }
            archetype->signature.infos = temp;

            archetype->signature.infos[archetype->signature.num_components].id = i;
            archetype->signature.infos[archetype->signature.num_components].size = ecs->component_infos[i].size;

            ++archetype->signature.num_components;
        }
    }

    // Allowing for empty archetype.
    if (archetype->signature.num_components > 0)
    {
        // Allocate for component lists.
        void** component_lists_temp = malloc(
            archetype->signature.num_components * sizeof(void*));

        if (!component_lists_temp)
        {
            // TODO: Handle failure.
            return INVALID_ARCHETYPE;
        }

        archetype->component_lists = component_lists_temp;

        // Initialise component list pointers to nullptr, otherwise when we try to realloc
        // they will have garbage values and fail.
        for (int i = 0; i < archetype->signature.num_components; ++i)
        {
            archetype->component_lists[i] = 0;
        }
    }

    // Add the archetype to all views that fit it's signature.
    const int num_views = Vector_size(ecs->views);
    for (int i = 0; i < num_views; ++i)
    {
        View* view = &ecs->views[i];
        if ((archetype_bitset & view->components_bitset) == view->components_bitset)
        {
            View_add_archetype(view, archetype_id);
        }
    }

    return archetype_id;
}

void ECS_move_archetype(ECS* ecs, EntityID id, ArchetypeID old_archetype_id,
    ArchetypeID new_archetype_id)
{
    // Add the entity to the new archetype.
    Archetype* new_archetype = &ecs->archetypes[new_archetype_id];
    Archetype_add_entity(ecs, new_archetype, id);

    // Copy where the entity used to live.
    const EntityIndex old_entity_index = ecs->entity_indices[id];
       
    // Get the index of the last entity in the archetype.
    int component_list_index = Vector_size(new_archetype->index_to_entity) - 1;

    // Update information on where the entity is.
    ecs->entity_indices[id].archetype_id = new_archetype_id;
    ecs->entity_indices[id].component_list_index = component_list_index;

    // Check for an old archetype.
    // NOTE: There should always be an old archetype except if the entity has just been
    //       created and is being moved to the empty archetype!
    Archetype* old_archetype = 0;
    if (old_archetype_id != INVALID_ARCHETYPE)
    {
        old_archetype = &ecs->archetypes[old_archetype_id];
    }
    if (!old_archetype)
    {
        assert(new_archetype_id == EMPTY_ARCHETYPE_ID);
        if (new_archetype_id == EMPTY_ARCHETYPE_ID)
        {
            // TODO: Handle error!!!
        }
        return;
    }

    // We want to find the matching components in the old and new archetypes,
    // copy from old to new if they match. Sadly we need an O(N^2) loop to
    // find the index of the components in the old archetype.
        
    // For each component in the new archetype, search the old archetype for a matching one.
    for (int i = 0; i < new_archetype->signature.num_components; ++i)
    {
        ComponentID cid_new = new_archetype->signature.infos[i].id;

        int old_component_list_i = -1;
        for (int j = 0; j < old_archetype->signature.num_components; ++j)
        {
            ComponentID cid_old = old_archetype->signature.infos[j].id;
            if (cid_new == cid_old) 
            {
                old_component_list_i = j;
                break;
            }
        }
            
        // Not found, so nothing to copy from old to new.
        if (old_component_list_i == -1) continue;

        const ComponentInfo info = ecs->component_infos[cid_new];

        // Convert void* to byte array so we can do pointer arithmetic.

        const void* vsrc = old_archetype->component_lists[old_component_list_i];
        void* vdest = new_archetype->component_lists[i];

        const uint8_t* src = (uint8_t*)(vsrc);
        uint8_t* dest = (uint8_t*)(vdest);

        const uint32_t src_offset = (uint32_t)(old_entity_index.component_list_index) * info.size;
        const uint32_t dest_offset = (uint32_t)(component_list_index) * info.size;

        // Copy the old data to the new archetype.
        memcpy(dest + dest_offset, src + src_offset, info.size);

        // Get location of entity in old archetype.
        // TODO: WHAT? WHY?
    }

    // Remove old data from archetype.
    Archetype_remove_entity(ecs, old_archetype, old_entity_index.component_list_index);
}

inline void Archetype_add_entity(const ECS* ecs, Archetype* archetype, 
    EntityID eid)
{
    // TODO: I fear we have some logic error here.
    //       If we remove an entity from the archetype, now we don't need
    //       to realloc for the entity as there is spare capacity, however,
    //       now there is 

    // TODO: Hack, not sure how else to do this.
    size_t old_capacity = Vector_capacity(archetype->index_to_entity);
    Vector_push_back(archetype->index_to_entity, eid);
    size_t new_capacity = Vector_capacity(archetype->index_to_entity);

    // TODO: clear/initialise data??
    if (old_capacity != new_capacity)
    {
        // Grow each component list.
        for (int i = 0; i < archetype->signature.num_components; ++i)
        {
            ComponentInfo info = archetype->signature.infos[i];

            void* temp = realloc(archetype->component_lists[i],
                new_capacity * info.size);

            if (!temp)
            {
                // TODO: handle failure
                return;
            }

            archetype->component_lists[i] = temp;
        }
    }
}

inline void Archetype_remove_entity(const ECS* ecs, Archetype* archetype, 
    int entity_index)
{
    // TODO: Implement vector remove functionality. Or some sort of function
    //       to do this? Removing won't really be what we want as that would
    //       shift everything rather than just quickly swapping.

    int num_entities = Vector_size(archetype->index_to_entity);

    // Archetype already empty, should not happen.
    if (num_entities == 0)
    {
        // TODO: Handle logic error?
        assert(0);
        return;
    }


    int last_entity_index = num_entities - 1;

    // Handle easy case of the entity being the last in the archetype.
    if (entity_index == last_entity_index)
    {
        // TODO: vector_pop_back functionality?
        CHDS_VectorHeader* h = CHDS_Vector_header(archetype->index_to_entity);
        --h->count;
        
        return;
    }

    // TODO: how do we update the old entitiy, need an index to id..... but in
    //       archetype damn it.

    // Copy the components from the last entity in the archetype over the top
    // of the one we're removing.
    for (int i = 0; i < archetype->signature.num_components; ++i)
    {
        uint8_t* component_list = (uint8_t*)(archetype->component_lists[i]);
        const uint32_t component_size = archetype->signature.infos[i].size;
        
        const uint32_t component_to_remove = entity_index * component_size;
        const uint32_t component_to_copy = last_entity_index * component_size;

        // Copy the last component over the component we're removing.
        memcpy(component_list + component_to_remove,
            component_list + component_to_copy,
            component_size);
    }

    // Update the entity we've moved's index.
    const EntityID entity_to_remove = archetype->index_to_entity[last_entity_index];
    archetype->index_to_entity[entity_index] = entity_to_remove;

    // Update the entity's index in the ecs to reflect it's been removed from its archetype.
    ecs->entity_indices[entity_to_remove].component_list_index = -1;
    ecs->entity_indices[entity_to_remove].archetype_id = INVALID_ARCHETYPE;

    // 'Remove' the last entity.
    // TODO: vector_pop_back functionality?
    CHDS_VectorHeader* h = CHDS_Vector_header(archetype->index_to_entity);
    --h->count;
}

