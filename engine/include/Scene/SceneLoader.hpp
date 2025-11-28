#pragma once

#include "Content/SceneFormat.hpp"
#include "ECS/EntityRegistry.hpp"
#include "Resource/Resource.hpp"
#include "RHI/RHIDevice.hpp"
#include <vector>
#include <utility>

namespace RenderToy
{
    /**
     * SceneLoader loads a SceneDescriptor into the EntityRegistry
     *
     * It handles:
     * - Entity creation with all components
     * - Rendering resource loading (Mesh, MaterialSet, Submesh, Material, etc.)
     * - Component data mapping from descriptors to ECS components
     */
    class SceneLoader
    {
    public:
        /**
         * Construct a SceneLoader
         *
         * @param registry Entity registry to load entities into
         * @param meshMgr Mesh resource manager
         * @param materialSetMgr MaterialSet resource manager
         * @param submeshMgr Submesh resource manager
         * @param materialMgr Material resource manager
         * @param device RHI device for GPU resource creation
         */
        SceneLoader(EntityRegistry& registry,
                   MeshManager& meshMgr,
                   MaterialSetManager& materialSetMgr,
                   SubmeshManager& submeshMgr,
                   MaterialManager& materialMgr,
                   RHIDevice* device);

        /**
         * Load a scene descriptor into the entity registry
         *
         * Creates entities with their components based on the scene descriptor.
         * Loaded entities are tracked internally for later clearing.
         *
         * @param scene Scene descriptor to load
         */
        void load(const SceneDescriptor& scene);

        /**
         * Clear all entities that were loaded by this SceneLoader
         *
         * Removes all entities that were created during load() calls.
         * Does not affect other entities in the registry.
         */
        void clear();

    private:
        EntityRegistry& registry_;
        MeshManager& meshMgr_;
        MaterialSetManager& materialSetMgr_;
        SubmeshManager& submeshMgr_;
        MaterialManager& materialMgr_;
        RHIDevice* device_;

        // Track loaded entities for clearing
        std::vector<EntityID> loadedEntities_;

        /**
         * Create a single entity from an entity descriptor
         *
         * @param desc Entity descriptor
         * @return Created entity ID
         */
        EntityID createEntity(const EntityDescriptor& desc);

        /**
         * Load Transform component
         */
        Transform createTransform(const TransformDescriptor& desc);

        /**
         * Load RenderObject resources (mesh and materials) from file
         * Returns pair of {MeshHandle, MaterialSetHandle}
         * Returns invalid handles if loading fails
         */
        std::pair<MeshHandle, MaterialSetHandle> loadRenderObject(const RenderObjectDescriptor& desc);

        /**
         * Create Camera component
         */
        Camera createCamera(const CameraDescriptor& desc);

        /**
         * Create Rigidbody component
         */
        Rigidbody createRigidbody(const RigidbodyDescriptor& desc);

        /**
         * Create SphereCollider component
         */
        SphereCollider createSphereCollider(const SphereColliderDescriptor& desc);

        /**
         * Create BoxCollider component
         */
        BoxCollider createBoxCollider(const BoxColliderDescriptor& desc);
    };
}
