#pragma once

#include <optional>
#include <string>
#include <vector>
#include "Primitives.hpp"

namespace RenderToy
{
    // ============================================================================
    // Component Descriptors
    // ============================================================================

    // Represents position, rotation, and scale in 3D space
    struct TransformDescriptor{
        Vec3 position = zeros();
        Vec4 rotation = unitQuat();
        Vec3 scale = ones();
    };

    // Material override descriptor for mesh rendering
    // Allows replacing material textures on a per-entity basis
    struct MaterialOverride {
        std::string baseColorTexture;  // URI to base color texture (e.g., "file:texture.png")
        std::string targetSlot;        // Material slot to override ("*" for all slots)
    };

    // Specifies which shader to use for rendering
    struct ShaderDescriptor {
        std::string modulePath;        // Path to shader module (e.g., "file:shader.metallib")
        std::string vertexFunction;    // Vertex shader entry point
        std::string fragmentFunction;  // Fragment shader entry point
    };

    // Describes a RenderObject component (mesh + materials + shader)
    struct RenderObjectDescriptor {
        std::string meshUri;  // "file:model.fbx" or "embedded:cube"
        std::vector<MaterialOverride> materialOverrides;
        ShaderDescriptor shader;
    };

    // Enables physics simulation for an entity
    struct RigidbodyDescriptor{
        Vec3 velocity = zeros();
        bool useGravity = true;
        double mass = 1.0f;
    };

    /// Physics material properties for colliders
    struct PhysicsMaterialDescriptor{
        double bounciness = 0.0f;  // Coefficient of restitution [0, 1]
        double friction = 0.5f;    // Coefficient of friction [0, 1]
    };

    // Axis-aligned or rotated box collision shape
    struct BoxColliderDescriptor{
        Vec3 center = zeros();
        Vec4 rotation = unitQuat();
        Vec3 size = ones();
        PhysicsMaterialDescriptor material;
    };

    // Spherical collision shape
    struct SphereColliderDescriptor{
        Vec3 center = zeros();
        double radius = 0.5f;
        PhysicsMaterialDescriptor material;
    };

    // Camera projection type
    enum class ProjectionType: uint8_t{
        Perspective = 0,
        Orthographic = 1
    };

    // Defines viewing parameters for rendering
    struct CameraDescriptor{
        CameraType cameraType = CameraType::MainCamera;
        ProjectionType projection = ProjectionType::Perspective;
        double fov = 45.0f;          // Field of view in degrees (for perspective)
        double nearPlane = 0.1f;
        double farPlane = 100.0f;

        // used when projection == Orthographic
        double orthoWidth = 10.0f;
        double orthoHeight = 10.0f;
    };

    /// Marker Component
    // Indicates this entity should receive player input
    struct PlayerDescriptor {};
    // Used for editor-only cameras
    struct EditorDescriptor {};

    // ============================================================================
    // Entity Descriptor
    // ============================================================================

    // Represents a single game object with attached components
    struct EntityDescriptor{
        std::string name;

        // Optional components (present if entity has them)
        std::optional<TransformDescriptor> transform;
        std::optional<RenderObjectDescriptor> renderObject;
        std::optional<RigidbodyDescriptor> rigidbody;
        std::optional<BoxColliderDescriptor> boxCollider;
        std::optional<SphereColliderDescriptor> sphereCollider;
        std::optional<CameraDescriptor> camera;
        std::optional<PlayerDescriptor> player;
        std::optional<EditorDescriptor> editor;

        // Check if entity has a specific component
        inline bool hasTransform()      const{ return transform.has_value(); }
        inline bool hasRenderObject()   const{ return renderObject.has_value(); }
        inline bool hasRigidbody()      const{ return rigidbody.has_value(); }
        inline bool hasBoxCollider()    const{ return boxCollider.has_value(); }
        inline bool hasSphereCollider() const{ return sphereCollider.has_value(); }
        inline bool hasCamera()         const{ return camera.has_value(); }
        inline bool hasPlayer()         const{ return player.has_value(); }
        inline bool hasEditor()         const{ return editor.has_value(); }
    };

    // ============================================================================
    // Scene Descriptor
    // ============================================================================

    // Contains all entities in a scene
    struct SceneDescriptor{
        std::vector<EntityDescriptor> entities;

        // Scene metadata (optional, can be expanded)
        std::string name;

        // Get entity by name (linear search, OK for editor/loading)
        inline const EntityDescriptor* findEntity(const std::string& entityName) const{
            for(const auto& entity : entities){
                if (entity.name == entityName)
                    return &entity;
            }
            return nullptr;
        }

        inline EntityDescriptor* findEntity(const std::string& entityName){
            return const_cast<EntityDescriptor*>(
                static_cast<const SceneDescriptor*>(this)->findEntity(entityName));
        }
    };
}
