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
    ComponentInfo* temp = realloc(ecs->component_infos, 
        (size_t)(ecs->num_components + 1) * sizeof(ComponentInfo));

    if (!temp)
    {
        // TODO: handle failure.
        return -1;
    }
    ecs->component_infos = temp;

    // Store the size of the component for resizing archetype arrays.
    ecs->component_infos[ecs->num_components].size = component_size;
    ecs->component_infos[ecs->num_components].id = ecs->num_components; // TODO: Do we need this?

    const ComponentID cid = ecs->num_components;

    ++ecs->num_components;

    return cid;
}

SystemID ECS_register_system(ECS* ecs)
{
    System* temp_systems = realloc(ecs->systems, (size_t)(ecs->num_systems + 1) * sizeof(System));
    if (!temp_systems)
    {
        // TODO: Handle failure
        return -1;
    }
    ecs->systems = temp_systems;

    // Initialise the new system
    SystemID system_id = ecs->num_systems;
    System* system = &ecs->systems[system_id];
    System_init(system);

    ++ecs->num_systems;

    return system_id;
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
    for (int i = 0; i < ecs->num_archetypes; ++i)
    {
        Archetype* archetype = &ecs->archetypes[i];
        if (archetype->signature.bitset == old_bitset)
        {
            Archetype_remove_entity(ecs, archetype, ecs->entity_indices[id].component_list_index);
            break;
        }
    }
}

void ECS_add_component(ECS* ecs, EntityID eid, ComponentID cid)
{
    const ComponentsBitset old_components_bitset = ecs->entity_components_bitsets[eid];
    const ComponentID component_bitset = COMPONENT_ID_TO_BITSET(cid);

    // Entity already has component.
    if (old_components_bitset & component_bitset)
    {
        return;
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
    for (int i = 0; i < ecs->num_archetypes; ++i)
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

    // TODO: Return void pointer to component?
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
    for (int i = 0; i < ecs->num_archetypes; ++i)
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

    int i = Archetype_find_component_list(archetype, cid);
    if (i == -1)
    {
        // TODO: Handle failure
        return 0;
    }

    //ComponentInfo cinfo = ecs->component_infos[cid];

    // Convert to uint8_t for pointer arithmetic.
    uint8_t* component_list = archetype->component_lists[i];
    
    void* component = component_list + ei.component_list_index * size;
    return component;
}

// Internal helper functions
ArchetypeID ECS_create_archetype(ECS* ecs, ComponentsBitset archetype_bitset)
{
    // Currently we're allowing for an empty archetype to keep the logic simple,
    // so that every entity lives in an archetype. May change in the future.
    Archetype* temp = realloc(ecs->archetypes,
        (size_t)(ecs->num_archetypes + 1) * sizeof(Archetype));

    if (!temp)
    {
        // TODO: handle failure.
        return INVALID_ARCHETYPE;
    }
    ecs->archetypes = temp;

    ArchetypeID archetype_id = ecs->num_archetypes;
    Archetype* archetype = &ecs->archetypes[archetype_id];
    ++ecs->num_archetypes;

    Archetype_init(archetype);
    archetype->signature.bitset = archetype_bitset;

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

    // Add the archetype to all systems that fit it's signature.
    for (int i = 0; i < ecs->num_systems; ++i)
    {
        System* system = &ecs->systems[i];
        if ((archetype_bitset & system->components_bitset) == system->components_bitset)
        {
            System_add_archetype(system, archetype_id);
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

    // Update information on where the entity is.
    ecs->entity_indices[id].archetype_id = new_archetype_id;
    ecs->entity_indices[id].component_list_index = new_archetype->entity_count - 1;

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
        const uint32_t dest_offset = (uint32_t)(new_archetype->entity_count - 1) * info.size;

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

    // Create space for the new entity, or reuse spare capacity
    if (archetype->entity_count == archetype->entity_capacity)
    {
        // TODO: Grow by factor.
        const float GROWTH_FACTOR = 0.25f;

        int extra = (int)(archetype->entity_capacity * GROWTH_FACTOR);
        if (extra == 0) extra = 1;

        archetype->entity_capacity += extra;

        // Grow each component list.
        for (int i = 0; i < archetype->signature.num_components; ++i)
        {
            ComponentInfo info = archetype->signature.infos[i];

            void* temp = realloc(archetype->component_lists[i],
                (size_t)(archetype->entity_capacity) * info.size);

            if (!temp)
            {
                // TODO: handle failure
                return;
            }

            archetype->component_lists[i] = temp;
            
        }

        // Grow index to entity map.
        int* temp = realloc(archetype->index_to_entity, 
            (size_t)(archetype->entity_capacity) * sizeof(int));

        if (!temp)
        {
            // TODO: Handle failure.
            return;
        }

        archetype->index_to_entity = temp;
    }

    // TODO: clear/initialise data??
    archetype->index_to_entity[archetype->entity_count] = eid;
     
    ++archetype->entity_count;
}

inline void Archetype_remove_entity(const ECS* ecs, Archetype* archetype, 
    int entity_index)
{
    // Archetype already empty, should not happen.
    if (archetype->entity_count == 0)
    {
        // TODO: Handle logic error?
        assert(0);
        return;
    }


    int last_entity_index = archetype->entity_count - 1;

    // Handle easy case of the entity being the last in the archetype.
    if (entity_index == last_entity_index)
    {
        --archetype->entity_count;
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

    // Update the entity's index in the ecs.
    ecs->entity_indices[entity_to_remove].component_list_index = entity_index;

    // 'Remove' the last entity.
    --archetype->entity_count;
}

