#include "Scene/SceneLoader.hpp"
#include "Log/Log.hpp"
#include "ECS/Component.hpp"
#include "Content/MeshFormat.hpp"
#include "Importer/MeshImporter.hpp"
#include <filesystem>
#include <fstream>

namespace RenderToy
{
    SceneLoader::SceneLoader(EntityRegistry& registry,
                           MeshManager& meshMgr,
                           MaterialSetManager& materialSetMgr,
                           SubmeshManager& submeshMgr,
                           MaterialManager& materialMgr,
                           RHIDevice* device)
        : registry_(registry)
        , meshMgr_(meshMgr)
        , materialSetMgr_(materialSetMgr)
        , submeshMgr_(submeshMgr)
        , materialMgr_(materialMgr)
        , device_(device)
    {
    }

    void SceneLoader::load(const SceneDescriptor& scene)
    {
        LOG_INFO(LOG_SCENE, "Loading scene with {} entities", scene.entities.size());

        for(const auto& entityDesc : scene.entities){
            EntityID id = createEntity(entityDesc);
            loadedEntities_.push_back(id);
        }

        LOG_INFO(LOG_SCENE, "Scene loaded successfully");
    }

    void SceneLoader::clear()
    {
        LOG_INFO(LOG_SCENE, "Clearing {} loaded entities", loadedEntities_.size());

        for(EntityID id : loadedEntities_){
            registry_.destroyEntity(id);
        }

        loadedEntities_.clear();
    }

    EntityID SceneLoader::createEntity(const EntityDescriptor& desc)
    {
        // Transform (use default if not provided)
        Transform transform = desc.transform.has_value()
            ? createTransform(*desc.transform)
            : Transform{.entity = {}, .isActive = true, .position = {}, .rotation = {}, .scale = {1,1,1}};

        // Create entity with Transform
        EntityID id = registry_.createEntity(transform);

        // Add optional components

        // RenderObject
        if(desc.renderObject.has_value()){
            auto [meshHandle, materialSetHandle] = loadRenderObject(*desc.renderObject);

            // Always add RenderObject if requested; inactive if resources failed to load
            bool isValid = meshHandle.isValid() && materialSetHandle.isValid();
            registry_.appendComponent(id, RenderObject{
                .entity = id,
                .isActive = isValid,
                .alpha = 1.0f,
                .mesh = meshHandle,
                .materialSet = materialSetHandle,
                .shader = {}  // TODO: Load shaders
            });
        }

        // Camera
        if(desc.camera.has_value()){
            Camera camera = createCamera(*desc.camera);
            camera.entity = id;
            registry_.appendComponent(id, std::move(camera));
        }

        // Rigidbody
        if(desc.rigidbody.has_value()){
            Rigidbody rb = createRigidbody(*desc.rigidbody);
            rb.entity = id;
            registry_.appendComponent(id, std::move(rb));
        }

        // SphereCollider
        if(desc.sphereCollider.has_value()){
            SphereCollider collider = createSphereCollider(*desc.sphereCollider);
            collider.entity = id;
            registry_.appendComponent(id, std::move(collider));
        }

        // BoxCollider
        if(desc.boxCollider.has_value()){
            BoxCollider collider = createBoxCollider(*desc.boxCollider);
            collider.entity = id;
            registry_.appendComponent(id, std::move(collider));
        }

        // Marker components

        if(desc.player.has_value()){
            Player player{.entity = id, .isActive = true};
            registry_.appendComponent(id, std::move(player));
        }

        if(desc.editor.has_value()){
            Editor editor{.entity = id, .isActive = true};
            registry_.appendComponent(id, std::move(editor));
        }

        // Note: Attachable, Climbable not in SceneFormat yet
        // TODO: Add to SceneFormat if needed

        return id;
    }

    Transform SceneLoader::createTransform(const TransformDescriptor& desc)
    {
        return Transform{
            .entity = {},  // Will be set when entity is created
            .isActive = true,
            .position = desc.position,
            .rotation = desc.rotation,
            .scale = desc.scale
        };
    }

    std::pair<MeshHandle, MaterialSetHandle> SceneLoader::loadRenderObject(const RenderObjectDescriptor& desc)
    {
        if(!device_){
            LOG_ERROR(LOG_SCENE, "Cannot load RenderObject: RHI device is null");
            return {{}, {}};
        }

        // Parse mesh URI and get file path
        std::filesystem::path filePath = desc.meshUri;

        // Load submeshes using SubmeshManager
        // First, determine how many submeshes are in the file
        auto ext = filePath.extension().string();
        std::optional<MeshData> meshData;

        if(ext == ".rtmesh"){
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            if(!file.is_open()){
                LOG_ERROR(LOG_SCENE, "Failed to open mesh file: {}", filePath.string());
                return {{}, {}};
            }

            auto fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> buffer(fileSize);
            file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
            file.close();

            meshData = deserializeMesh(buffer);
        }
        else if(ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".pmx"){
            meshData = importMesh(filePath.string());
        }
        else{
            LOG_ERROR(LOG_SCENE, "Unsupported mesh file format: {}", ext);
            return {{}, {}};
        }

        if(!meshData.has_value() || meshData->submeshes.empty()){
            LOG_ERROR(LOG_SCENE, "Failed to import mesh or mesh is empty: {}", filePath.string());
            return {{}, {}};
        }

        // Load each submesh using SubmeshManager
        std::vector<SubmeshHandle> submeshHandles;

        for(uint32_t i = 0; i < meshData->submeshes.size(); ++i){
            SubmeshRequest submeshReq{
                .path = filePath,
                .submeshIndex = i,
                .device = device_
            };

            auto submeshHandle = submeshMgr_.getOrLoad(submeshReq);
            if(!submeshHandle.isValid()){
                LOG_ERROR(LOG_SCENE, "Failed to load submesh {}", i);
                return {{}, {}};
            }

            submeshHandles.push_back(submeshHandle);
        }

        // Create default materials for each submesh
        std::vector<MaterialHandle> materialHandles;

        for(uint32_t i = 0; i < submeshHandles.size(); ++i){
            Material defaultMaterial{
                .albedo = Vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .metallic = 0.0f,
                .roughness = 0.5f,
                .alpha = 1.0f
            };

            // Create key for material
            MaterialKey materialKey{
                .canonicalPath = std::filesystem::weakly_canonical(filePath).string(),
                .materialIndex = i
            };

            auto materialHandle = materialMgr_.insert(materialKey, std::move(defaultMaterial));
            materialHandles.push_back(materialHandle);
        }

        // Create Mesh (logical grouping)
        Mesh mesh{.submeshes = submeshHandles};

        // Create key for mesh
        MeshKey meshKey{
            .canonicalPath = std::filesystem::weakly_canonical(filePath).string()
        };

        auto meshHandle = meshMgr_.insert(meshKey, std::move(mesh));

        // Create MaterialSet
        MaterialSet materialSet{.materials = materialHandles};

        // Create key for material set
        MaterialSetKey materialSetKey{
            .canonicalPath = std::filesystem::weakly_canonical(filePath).string()
        };

        auto materialSetHandle = materialSetMgr_.insert(materialSetKey, std::move(materialSet));

        LOG_INFO(LOG_SCENE, "Loaded mesh: {} ({} submeshes)",
                filePath.string(), submeshHandles.size());

        return {meshHandle, materialSetHandle};
    }

    Camera SceneLoader::createCamera(const CameraDescriptor& desc){
        // Map ProjectionType to Projection
        Projection proj = (desc.projection == ProjectionType::Perspective)
            ? Projection::PERSPECTIVE
            : Projection::ORTHOGRAPHIC;

        return Camera{
            .entity = {},  // Will be set when entity is created
            .isActive = true,
            .type = desc.cameraType,
            .fov = static_cast<float>(desc.fov),
            .nearPlane = static_cast<float>(desc.nearPlane),
            .farPlane = static_cast<float>(desc.farPlane),
            .proj = proj
        };
    }

    Rigidbody SceneLoader::createRigidbody(const RigidbodyDescriptor& desc)
    {
        return Rigidbody{
            .entity = {},  // Will be set when entity is created
            .isActive = true,
            .velocity = desc.velocity,
            .useGravity = desc.useGravity,
            .mass = static_cast<float>(desc.mass)
        };
    }

    SphereCollider SceneLoader::createSphereCollider(const SphereColliderDescriptor& desc){
        return SphereCollider{
            .entity = {},  // Will be set when entity is created
            .isActive = true,
            .position = desc.center,
            .radius = static_cast<float>(desc.radius),
            .material = PhysicsMaterial{
                .bounciness = static_cast<float>(desc.material.bounciness),
                .friction = static_cast<float>(desc.material.friction)
            }
        };
    }

    BoxCollider SceneLoader::createBoxCollider(const BoxColliderDescriptor& desc){
        return BoxCollider{
            .entity = {},  // Will be set when entity is created
            .isActive = true,
            .position = desc.center,
            .rotation = unitQuat(),
            .scale = desc.size,
            .material = PhysicsMaterial{
                .bounciness = static_cast<float>(desc.material.bounciness),
                .friction = static_cast<float>(desc.material.friction)
            }
        };
    }
}
