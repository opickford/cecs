#include "ecs_internal.h"

#include "archetype_internal.h"
#include "view_internal.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// TODO: Need to be returning some error code or at least document it.

static CECS_ArchetypeId cecs_create_archetype(CECS* ecs, 
    CECS_ComponentsBitset archetype_bitset);

static void cecs_move_archetype(CECS* ecs, 
    CECS_EntityId id, 
    CECS_ArchetypeId old_archetype_id,
    CECS_ArchetypeId new_archetype_id);

static void cecs_archetype_add_entity(const CECS* ecs, 
    CECS_Archetype* archetype,
    CECS_EntityId eid);

static void cecs_archetype_remove_entity(CECS* ecs, 
    CECS_Archetype* archetype,
    int entity_index);

// CECS API
CECS* cecs_create()
{
    CECS* ecs = calloc(1, sizeof(CECS));
    
    // Ensure an empty archetype exists to store entities without components.
    // NOTE: This logic may be refactored in the future so that there is no 
    //       need for this empty archetype! The issue being removing an entity's
    //       last component. Also, keeping this simplifies other logic as it means
    //       an entity always has an archetype!
    CECS_ArchetypeId empty_archetype = cecs_create_archetype(ecs, 0);

    // Ensure the archetype is valid and matches the hardcoded index.
    if (empty_archetype == INVALID_ARCHETYPE || empty_archetype != EMPTY_ARCHETYPE_ID)
    {
        // TODO: Handle error!
        assert(0);
    }

    return ecs;
}

CECS_ComponentId cecs_register_component(CECS* ecs, uint32_t component_size)
{
    // Store the size of the component for resizing archetype arrays.
    CECS_ComponentInfo ci = {
        .size = component_size,
        .id = (CECS_ComponentId)chds_vec_size(ecs->component_infos)
    };
    chds_vec_push(ecs->component_infos, ci);

    return ci.id;
}

// TODO: Some function for passing in component ids separately to create bitsets.
CECS_ViewId cecs_view_create(CECS* ecs, CECS_ComponentsBitset include, CECS_ComponentsBitset exclude)
{
    // TODO: Should write tests for this sort of thing!
    // Include and exclude cannot have matching bits.
    if ((include & exclude) != 0)
    {
        return INVALID_VIEW;
    }

    CECS_ViewId num_views = (CECS_ViewId)chds_vec_size(ecs->views);

    // Look for existing view.
    // TODO: Map would be nicer.
    for (int i = 0; i < num_views; ++i)
    {
        CECS_View* v = &ecs->views[i];
        if (v->include == include &&
            v->exclude == exclude)
        {
            return i;
        }
    }

    chds_vec_push(ecs->views, ((CECS_View) { 
        .include = include, 
        .exclude = exclude 
    }));

    CECS_View* v = &ecs->views[num_views];

    // Load matching archetypes into ecs.
    size_t num_archetypes = chds_vec_size(ecs->archetypes);
    for (int aid = 0; aid < num_archetypes; ++aid)
    {
        const CECS_ComponentsBitset bits = ecs->archetypes[aid].signature.bitset;
        
        // TODO: Some helper function for this?
        // CECS_Archetype bitset must have at least the include bits but
        // none of the exclude ones.
        if ((bits & include) == include &&
            (bits & exclude) == 0)
        {
            chds_vec_push(v->archetype_ids, aid);
        }
    }

    return num_views;
}

CECS_ViewIter cecs_view_iter_create(const CECS* ecs, const CECS_ViewId vid)
{
    CECS_View* view = &ecs->views[vid];

    uint32_t num_archetypes = (uint32_t)chds_vec_size(view->archetype_ids);
    uint32_t num_entities = 0;

    // Avoid ptr arithmetic on nullptr.
    CECS_ArchetypeId* start = 0;

    if (num_archetypes > 0)
    {
        start = view->archetype_ids - 1; // Offset by 1 because of initial increment.
        num_entities = (uint32_t)chds_vec_size(ecs->archetypes[view->archetype_ids[0]].index_to_entity);
    }

    CECS_ViewIter it = {
        .ecs = ecs,
        .vid = vid,
        .aid = start,
        .rem = num_archetypes,
        .num_entities = num_entities
    };
    return it;
}

int cecs_view_iter_next(CECS_ViewIter* it)
{
    if (it->rem == 0) return 0;
    --it->rem;

    // Move to next archetype.
    ++it->aid;

    it->num_entities = (uint32_t)chds_vec_size(it->ecs->archetypes[*it->aid].index_to_entity);

    return 1;
}

void* cecs_get_column(CECS_ViewIter it, CECS_ComponentId cid)
{
    return cecs_archetype_get_column(&it.ecs->archetypes[*it.aid], cid);
}

CECS_EntityId cecs_create_entity(CECS* ecs)
{
    CECS_EntityId entity;

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
        CECS_ComponentsBitset* temp_bitsets = realloc(ecs->entity_components_bitsets,
            total_created_entities * sizeof(CECS_ComponentsBitset));

        if (!temp_bitsets)
        {
            printf("Failed to alloc for ecs->entity_components_bitsets.\n");
            return INVALID_ENTITY;
        }
        ecs->entity_components_bitsets = temp_bitsets;

        CECS_EntityIndex* temp_indices = realloc(ecs->entity_indices, 
            total_created_entities * sizeof(CECS_EntityIndex));
        if (!temp_indices)
        {
            printf("Failed to alloc for ecs->entity_indices.\n");
            return INVALID_ENTITY;
        }
        ecs->entity_indices = temp_indices;

        entity = total_created_entities - 1;   
    }
    
    // Initialise entity data.
    ecs->entity_components_bitsets[entity] = CECS_EMPTY_COMPONENTS_BITSET;
    ecs->entity_indices[entity].archetype_id = -1;
    ecs->entity_indices[entity].column = -1;

    // Move the entity to the empty archetype.
    cecs_move_archetype(ecs, entity, INVALID_ARCHETYPE, EMPTY_ARCHETYPE_ID);
    
    return entity;
}

// TODO: Rename destroy?
void cecs_destroy_entity(CECS* ecs, CECS_EntityId id)
{
    --ecs->num_used_entities;

    // Entities don't need to be packed as we will never be iterating over the entities loop.

    // Clear entity signature.
    const CECS_ComponentsBitset old_bitset = ecs->entity_components_bitsets[id];
    ecs->entity_components_bitsets[id] = CECS_EMPTY_COMPONENTS_BITSET;

    // Grow capacity if needed.
    if (ecs->free_entities_count == ecs->free_entities_capacity)
    {
        // TODO: Grow by factor?
        ++ecs->free_entities_capacity;

        CECS_EntityId* temp_free_entities = realloc(ecs->free_entities, ecs->free_entities_capacity * sizeof(CECS_EntityId));
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
    size_t num_archetypes = chds_vec_size(ecs->archetypes);
    for (int i = 0; i < num_archetypes; ++i)
    {
        CECS_Archetype* archetype = &ecs->archetypes[i];
        if (archetype->signature.bitset == old_bitset)
        {
            cecs_archetype_remove_entity(ecs, archetype, ecs->entity_indices[id].column);
            break;
        }
    }
}

void* cecs_add_component(CECS* ecs, CECS_EntityId eid, CECS_ComponentId cid)
{
    const CECS_ComponentsBitset old_components_bitset = ecs->entity_components_bitsets[eid];
    const CECS_ComponentId component_bitset = CECS_COMPONENT_ID_TO_BITSET(cid);

    // Entity already has component, return that component instead.
    if (old_components_bitset & component_bitset)
    {
        return cecs_get_component(ecs, eid, cid);
    }
 
    // Update entity's signature with the new component.
    ecs->entity_components_bitsets[eid] |= component_bitset;

    // TODO: If matching archetype doesn't exist, create one.
    // TODO: With this, maybe we don't want an archetype for just one component?
    //       Somehow we could delay archetype creation. (flag for it?)

    // TODO: Is there a nicer way to do this using some sort of mapping, brute 
    //       force search not ideal but shouldn't be a big issue for now.

    // Must use CECS_ArchetypeId as if we create a new archetype we will invalidate the pointers.
    CECS_ArchetypeId new_archetype_id = INVALID_ARCHETYPE;
    CECS_ArchetypeId old_archetype_id = INVALID_ARCHETYPE;
    const size_t num_archetypes = chds_vec_size(ecs->archetypes);
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
        new_archetype_id = cecs_create_archetype(ecs, ecs->entity_components_bitsets[eid]);
    }

    cecs_move_archetype(ecs, eid, old_archetype_id, new_archetype_id);

    // Return pointer to new component.
    return cecs_get_component(ecs, eid, cid);
}

void cecs_remove_component(CECS* ecs, CECS_EntityId eid, CECS_ComponentId cid)
{
    const CECS_ComponentsBitset old_components_bitset = ecs->entity_components_bitsets[eid];
    const CECS_ComponentId component_bitset = CECS_COMPONENT_ID_TO_BITSET(cid);

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

    // Must use CECS_ArchetypeId as if we create a new archetype we will invalidate the pointers.
    CECS_ArchetypeId new_archetype_id = INVALID_ARCHETYPE;
    CECS_ArchetypeId old_archetype_id = INVALID_ARCHETYPE;
    const size_t num_archetypes = chds_vec_size(ecs->archetypes);
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
        new_archetype_id = cecs_create_archetype(ecs, ecs->entity_components_bitsets[eid]);
    }

    cecs_move_archetype(ecs, eid, old_archetype_id, new_archetype_id);
}

void* cecs_get_component(CECS* ecs, CECS_EntityId eid, CECS_ComponentId cid)
{
    CECS_EntityIndex ei = ecs->entity_indices[eid];
    CECS_Archetype* archetype = &ecs->archetypes[ei.archetype_id];
    
    int size = ecs->component_infos[cid].size;

    // Convert to uint8_t for pointer arithmetic.
    uint8_t* column = cecs_archetype_get_column(archetype, cid);
    if (column == 0)
    {
        // TODO: Handle failure
        return 0;
    }
    
    void* component = column + ei.column * size;
    return component;
}

// Internal helper functions
// TODO: Should these private functions be moved elsewhere? They're not
//       intended to be part of the public api.
static CECS_ArchetypeId cecs_create_archetype(CECS* ecs, 
    CECS_ComponentsBitset archetype_bitset)
{
    // Currently we're allowing for an empty archetype to keep the logic simple,
    // so that every entity lives in an archetype. May change in the future.
    CECS_ArchetypeId archetype_id = (CECS_ArchetypeId)chds_vec_size(ecs->archetypes);
    CECS_Archetype new_archetype = {
        .signature = {
            .bitset = archetype_bitset
        }
    };

    chds_vec_push(ecs->archetypes, new_archetype);
    CECS_Archetype* archetype = &ecs->archetypes[archetype_id];

    // TODO: Remove cecs_archetype_init?
    //cecs_archetype_init(archetype);

    // Create archetype signature from bitset.
    for (int i = 0; i < CECS_MAX_COMPONENTS; ++i)
    {
        CECS_ComponentsBitset bitset = CECS_COMPONENT_ID_TO_BITSET(i);

        // Check if the archetype should have this component.
        if (bitset & archetype_bitset)
        {
            CECS_ComponentInfo* temp = realloc(archetype->signature.infos,
                (size_t)(archetype->signature.num_components + 1) * 
                sizeof(CECS_ComponentInfo));

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
        // Allocate for component columns.
        void** columns_temp = malloc(
            archetype->signature.num_components * sizeof(void*));

        if (!columns_temp)
        {
            // TODO: Handle failure.
            return INVALID_ARCHETYPE;
        }

        archetype->columns = columns_temp;

        // Initialise component list pointers to nullptr, otherwise when we try to realloc
        // they will have garbage values and fail.
        for (int i = 0; i < archetype->signature.num_components; ++i)
        {
            archetype->columns[i] = 0;
        }
    }

    // Add the archetype to all views that fit it's signature.
    const size_t num_views = chds_vec_size(ecs->views);
    for (int i = 0; i < num_views; ++i)
    {
        CECS_View* view = &ecs->views[i];

        if ((archetype_bitset & view->include) == view->include &&
            (archetype_bitset & view->exclude) == 0)
        {
            chds_vec_push(view->archetype_ids, archetype_id);
        }
    }

    return archetype_id;
}

static void cecs_move_archetype(CECS* ecs, CECS_EntityId id, CECS_ArchetypeId old_archetype_id,
    CECS_ArchetypeId new_archetype_id)
{
    // Add the entity to the new archetype.
    CECS_Archetype* new_archetype = &ecs->archetypes[new_archetype_id];
    cecs_archetype_add_entity(ecs, new_archetype, id);
    // TODO: Confirm this succeeeded?

    // Copy where the entity used to live.
    const CECS_EntityIndex old_entity_index = ecs->entity_indices[id];
       
    // Get the index of the last entity in the archetype.
    int column = (int)chds_vec_size(new_archetype->index_to_entity) - 1;

    // Update information on where the entity is.
    ecs->entity_indices[id].archetype_id = new_archetype_id;
    ecs->entity_indices[id].column = column;

    // Check for an old archetype.
    // NOTE: There should always be an old archetype except if the entity has just been
    //       created and is being moved to the empty archetype!
    CECS_Archetype* old_archetype = 0;
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
        CECS_ComponentId cid_new = new_archetype->signature.infos[i].id;

        int old_component_list_i = -1;
        for (int j = 0; j < old_archetype->signature.num_components; ++j)
        {
            CECS_ComponentId cid_old = old_archetype->signature.infos[j].id;
            if (cid_new == cid_old) 
            {
                old_component_list_i = j;
                break;
            }
        }
            
        // Not found, so nothing to copy from old to new.
        if (old_component_list_i == -1) continue;

        const CECS_ComponentInfo info = ecs->component_infos[cid_new];

        // Convert void* to byte array so we can do pointer arithmetic.

        const void* vsrc = old_archetype->columns[old_component_list_i];
        void* vdest = new_archetype->columns[i];

        const uint8_t* src = (uint8_t*)(vsrc);
        uint8_t* dest = (uint8_t*)(vdest);

        const uint32_t src_offset = (uint32_t)(old_entity_index.column) * info.size;
        const uint32_t dest_offset = (uint32_t)(column) * info.size;

        // Copy the old data to the new archetype.
        memcpy(dest + dest_offset, src + src_offset, info.size);

        // Get location of entity in old archetype.
        // TODO: WHAT? WHY?
    }

    // Remove old data from archetype.
    cecs_archetype_remove_entity(ecs, old_archetype, old_entity_index.column);
}

static void cecs_archetype_add_entity(const CECS* ecs, CECS_Archetype* archetype, 
    CECS_EntityId eid)
{
    // TODO: Move to archetype.c

    // TODO: I fear we have some logic error here.
    //       If we remove an entity from the archetype, now we don't need
    //       to realloc for the entity as there is spare capacity, however,
    //       now there is 

    // TODO: Hack, not sure how else to do this.
    size_t old_capacity = chds_vec_capacity(archetype->index_to_entity);
    chds_vec_push(archetype->index_to_entity, eid);
    size_t new_capacity = chds_vec_capacity(archetype->index_to_entity);

    // TODO: clear/initialise data??
    if (old_capacity != new_capacity)
    {
        // Grow each component list.
        for (int i = 0; i < archetype->signature.num_components; ++i)
        {
            CECS_ComponentInfo info = archetype->signature.infos[i];

            void* temp = realloc(archetype->columns[i],
                new_capacity * info.size);

            if (!temp)
            {
                // TODO: handle failure
                return;
            }

            archetype->columns[i] = temp;
        }
    }
}

static void cecs_archetype_remove_entity(CECS* ecs, CECS_Archetype* archetype, 
    int entity_index)
{
    // TODO: Implement CHDS_VEC remove functionality. Or some sort of function
    //       to do this? Removing won't really be what we want as that would
    //       shift everything rather than just quickly swapping.

    int num_entities = (int)chds_vec_size(archetype->index_to_entity);

    // CECS_Archetype already empty, should not happen.
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
        chds_vec_pop(archetype->index_to_entity);
        return;
    }

    // TODO: how do we update the old entitiy, need an index to id..... but in
    //       archetype damn it.

    // Copy the components from the last entity in the archetype over the top
    // of the one we're removing.
    for (int i = 0; i < archetype->signature.num_components; ++i)
    {
        uint8_t* component_list = (uint8_t*)(archetype->columns[i]);
        const uint32_t component_size = archetype->signature.infos[i].size;
        
        const uint32_t component_to_remove = entity_index * component_size;
        const uint32_t component_to_copy = last_entity_index * component_size;

        // Copy the last component over the component we're removing.
        memcpy(component_list + component_to_remove,
            component_list + component_to_copy,
            component_size);
    }

    // Update the entity we've moved's index.
    const CECS_EntityId entity_to_remove = archetype->index_to_entity[last_entity_index];
    archetype->index_to_entity[entity_index] = entity_to_remove;

    // TODO: Should this ecs stuff be done elsewhere??????

    // Update the entity's index in the ecs to reflect it's been removed from its archetype.
    ecs->entity_indices[entity_to_remove].column = -1;
    ecs->entity_indices[entity_to_remove].archetype_id = INVALID_ARCHETYPE;

    // 'Remove' the last entity.
    chds_vec_pop(archetype->index_to_entity);
}

