#include <gtest/gtest.h>
#include "Scene/SceneLoader.hpp"
#include "Content/SceneFormat.hpp"
#include "ECS/EntityRegistry.hpp"
#include "Resource/Resource.hpp"
#include "RHI/RHIDevice.hpp"
#include <memory>

using namespace RenderToy;

// Test fixture for SceneLoader tests
class SceneLoaderTest : public ::testing::Test {
protected:
    std::unique_ptr<RHIDevice> device;
    std::unique_ptr<EntityRegistry> registry;
    std::unique_ptr<MeshManager> meshManager;
    std::unique_ptr<MaterialSetManager> materialSetManager;
    std::unique_ptr<SubmeshManager> submeshManager;
    std::unique_ptr<MaterialManager> materialManager;
    std::unique_ptr<SceneLoader> loader;

    void SetUp() override {
        // Create RHI device
        RHIDeviceCreateDesc desc{
            .enableDebugLayer = false,
            .enableGPUValidation = false,
            .enableValidation = false,
            .applicationName = "SceneLoaderTest",
            .windowHandle = nullptr
        };
        device = RHIDevice::create(desc);
        ASSERT_NE(device, nullptr);

        // Create ECS registry
        registry = std::make_unique<EntityRegistry>();

        // Create resource managers
        meshManager = std::make_unique<MeshManager>();
        materialSetManager = std::make_unique<MaterialSetManager>();
        submeshManager = std::make_unique<SubmeshManager>();
        materialManager = std::make_unique<MaterialManager>();

        // Create scene loader
        loader = std::make_unique<SceneLoader>(
            *registry,
            *meshManager,
            *materialSetManager,
            *submeshManager,
            *materialManager,
            device.get()
        );
    }

    void TearDown() override {
        loader.reset();
        materialManager.reset();
        submeshManager.reset();
        materialSetManager.reset();
        meshManager.reset();
        registry.reset();
        device.reset();
    }
};

// ============================================================================
// Basic Scene Loading Tests
// ============================================================================

TEST_F(SceneLoaderTest, LoadEmptyScene){
    SceneDescriptor scene;
    scene.entities.clear();

    // Should not crash on empty scene
    EXPECT_NO_THROW(loader->load(scene));

    // Registry should be empty
    auto view = registry->query<Transform>();
    EXPECT_EQ(view.size(), 0);
}

TEST_F(SceneLoaderTest, LoadSingleEntityWithTransform){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "TestEntity";
    entity.transform = TransformDescriptor{
        .position = Vec3{1.0f, 2.0f, 3.0f},
        .rotation = Vec4{0.0f, 0.0f, 0.0f, 1.0f},
        .scale = Vec3{1.0f, 1.0f, 1.0f}
    };

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have one entity
    auto view = registry->query<Transform>();
    EXPECT_EQ(view.size(), 1);

    // Check transform values
    for(auto [id, archetype, transform] : view){
        EXPECT_FLOAT_EQ(transform.position.x, 1.0f);
        EXPECT_FLOAT_EQ(transform.position.y, 2.0f);
        EXPECT_FLOAT_EQ(transform.position.z, 3.0f);
    }
}

TEST_F(SceneLoaderTest, LoadEntityWithMesh){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "MeshEntity";
    entity.transform = TransformDescriptor{};
    entity.renderObject = RenderObjectDescriptor{
        .meshUri = "engine/test/Importer/test_data/simple_triangle.obj"
    };

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Transform and RenderObject
    auto view = registry->query<Transform, RenderObject>();
    EXPECT_EQ(view.size(), 1);

    // Check that mesh was loaded
    for(const auto& [id, archetype, transform, renderObj] : view){
        EXPECT_TRUE(renderObj.mesh.isValid());
    }
}

TEST_F(SceneLoaderTest, LoadEntityWithCamera){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "CameraEntity";
    entity.transform = TransformDescriptor{};
    entity.camera = CameraDescriptor{
        .projection = ProjectionType::Perspective,
        .fov = 60.0f,
        .nearPlane = 0.1f,
        .farPlane = 1000.0f
    };

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Transform and Camera
    auto view = registry->query<Transform, Camera>();
    EXPECT_EQ(view.size(), 1);

    // Check camera values
    for(const auto& [id, archetype, transform, camera]: view){
        EXPECT_EQ(camera.proj, Projection::PERSPECTIVE);
        EXPECT_FLOAT_EQ(camera.fov, 60.0f);
        EXPECT_FLOAT_EQ(camera.nearPlane, 0.1f);
        EXPECT_FLOAT_EQ(camera.farPlane, 1000.0f);
    }
}

TEST_F(SceneLoaderTest, LoadEntityWithRigidbody){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "PhysicsEntity";
    entity.transform = TransformDescriptor{};
    entity.rigidbody = RigidbodyDescriptor{
        .velocity = Vec3{1.0f, 0.0f, 0.0f},
        .useGravity = true,
        .mass = 10.0f
    };

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Transform and Rigidbody
    auto view = registry->query<Transform, Rigidbody>();
    EXPECT_EQ(view.size(), 1);

    // Check rigidbody values
    for(auto [id, archetype, transform, rb] : view){
        EXPECT_FLOAT_EQ(rb.velocity.x, 1.0f);
        EXPECT_TRUE(rb.useGravity);
        EXPECT_FLOAT_EQ(rb.mass, 10.0f);
    }
}

TEST_F(SceneLoaderTest, LoadEntityWithSphereCollider){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "SphereEntity";
    entity.transform = TransformDescriptor{};
    entity.sphereCollider = SphereColliderDescriptor{
        .center = Vec3{0.0f, 1.0f, 0.0f},
        .radius = 0.5f
    };

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Transform and SphereCollider
    auto view = registry->query<Transform, SphereCollider>();
    EXPECT_EQ(view.size(), 1);

    // Check collider values
    for(auto [id, archetype, transform, collider] : view){
        EXPECT_FLOAT_EQ(collider.position.x, 0.0f);
        EXPECT_FLOAT_EQ(collider.position.y, 1.0f);
        EXPECT_FLOAT_EQ(collider.position.z, 0.0f);
        EXPECT_FLOAT_EQ(collider.radius, 0.5f);
    }
}

TEST_F(SceneLoaderTest, LoadEntityWithBoxCollider){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "BoxEntity";
    entity.transform = TransformDescriptor{};
    entity.boxCollider = BoxColliderDescriptor{
        .center = Vec3{0.0f, 0.0f, 0.0f},
        .size = Vec3{2.0f, 2.0f, 2.0f}
    };

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Transform and BoxCollider
    auto view = registry->query<Transform, BoxCollider>();
    EXPECT_EQ(view.size(), 1);

    // Check collider values
    for(auto [id, archetype, transform, collider] : view){
        EXPECT_FLOAT_EQ(collider.scale.x, 2.0f);
        EXPECT_FLOAT_EQ(collider.scale.y, 2.0f);
        EXPECT_FLOAT_EQ(collider.scale.z, 2.0f);
    }
}

// ============================================================================
// Marker Component Tests
// ============================================================================

TEST_F(SceneLoaderTest, LoadEntityWithPlayerMarker){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "PlayerEntity";
    entity.transform = TransformDescriptor{};
    entity.player = PlayerDescriptor{};

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Player component
    auto view = registry->query<Player>();
    EXPECT_EQ(view.size(), 1);
}

TEST_F(SceneLoaderTest, LoadEntityWithEditorMarker){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "EditorEntity";
    entity.transform = TransformDescriptor{};
    entity.editor = EditorDescriptor{};

    scene.entities.push_back(entity);

    loader->load(scene);

    // Should have entity with Editor component
    auto view = registry->query<Editor>();
    EXPECT_EQ(view.size(), 1);
}

// ============================================================================
// Multi-Entity Scene Tests
// ============================================================================

TEST_F(SceneLoaderTest, LoadMultipleEntities){
    SceneDescriptor scene;

    // Create 5 entities with different components
    for(int i = 0; i < 5; ++i){
        EntityDescriptor entity;
        entity.name = "Entity_" + std::to_string(i);
        entity.transform = TransformDescriptor{
            .position = Vec3{static_cast<float>(i), 0.0f, 0.0f}
        };
        scene.entities.push_back(entity);
    }

    loader->load(scene);

    // Should have 5 entities
    auto view = registry->query<Transform>();
    EXPECT_EQ(view.size(), 5);
}

TEST_F(SceneLoaderTest, LoadComplexScene){
    SceneDescriptor scene;

    // Player entity
    {
        EntityDescriptor player;
        player.name = "Player";
        player.transform = TransformDescriptor{
            .position = Vec3{0.0f, 1.0f, 0.0f}
        };
        player.renderObject = RenderObjectDescriptor{
            .meshUri = "engine/test/Importer/test_data/simple_cube.obj"
        };
        player.rigidbody = RigidbodyDescriptor{
            .velocity = Vec3{0.0f, 0.0f, 0.0f},
            .useGravity = true,
            .mass = 1.0f
        };
        player.sphereCollider = SphereColliderDescriptor{
            .center = Vec3{0.0f, 0.0f, 0.0f},
            .radius = 0.5f
        };
        player.player = PlayerDescriptor{};
        scene.entities.push_back(player);
    }

    // Camera entity
    {
        EntityDescriptor camera;
        camera.name = "Camera";
        camera.transform = TransformDescriptor{
            .position = Vec3{0.0f, 2.0f, 5.0f}
        };
        camera.camera = CameraDescriptor{
            .projection = ProjectionType::Perspective,
            .fov = 60.0f,
            .nearPlane = 0.1f,
            .farPlane = 1000.0f
        };
        scene.entities.push_back(camera);
    }

    // Ground entity
    {
        EntityDescriptor ground;
        ground.name = "Ground";
        ground.transform = TransformDescriptor{
            .position = Vec3{0.0f, -1.0f, 0.0f},
            .scale = Vec3{10.0f, 1.0f, 10.0f}
        };
        ground.renderObject = RenderObjectDescriptor{
            .meshUri = "engine/test/Importer/test_data/simple_cube.obj"
        };
        ground.boxCollider = BoxColliderDescriptor{
            .center = Vec3{0.0f, 0.0f, 0.0f},
            .size = Vec3{10.0f, 1.0f, 10.0f}
        };
        scene.entities.push_back(ground);
    }

    loader->load(scene);

    // Check total entity count
    auto allEntities = registry->query<Transform>();
    EXPECT_EQ(allEntities.size(), 3);

    // Check specific components exist
    auto playerView = registry->query<Player, Rigidbody, SphereCollider>();
    EXPECT_EQ(playerView.size(), 1);

    auto cameraView = registry->query<Camera>();
    EXPECT_EQ(cameraView.size(), 1);

    auto groundView = registry->query<RenderObject, BoxCollider>();
    EXPECT_EQ(groundView.size(), 1);
}

// ============================================================================
// Resource Loading Tests
// ============================================================================

TEST_F(SceneLoaderTest, MeshResourceCaching){
    SceneDescriptor scene;

    // Create two entities with same mesh
    for(int i = 0; i < 2; ++i){
        EntityDescriptor entity;
        entity.name = "Entity_" + std::to_string(i);
        entity.transform = TransformDescriptor{};
        entity.renderObject = RenderObjectDescriptor{
            .meshUri = "engine/test/Importer/test_data/simple_triangle.obj"
        };
        scene.entities.push_back(entity);
    }

    loader->load(scene);

    // Both entities should share the same mesh handle (cached)
    auto view = registry->query<RenderObject>();
    ASSERT_EQ(view.size(), 2);

    generic_handle<Mesh> firstHandle;
    bool first = true;
    for(const auto& [id, archetype, renderObj] : view){
        if(first){
            firstHandle = renderObj.mesh;
            first = false;
        } else {
            // Should be same handle (cached)
            EXPECT_EQ(renderObj.mesh, firstHandle);
        }
    }
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(SceneLoaderTest, LoadEntityWithInvalidMesh){
    SceneDescriptor scene;

    EntityDescriptor entity;
    entity.name = "InvalidMeshEntity";
    entity.transform = TransformDescriptor{};
    entity.renderObject = RenderObjectDescriptor{
        .meshUri = "nonexistent_file.obj",
    };

    scene.entities.push_back(entity);

    // Should not crash, but mesh handle should be invalid
    EXPECT_NO_THROW(loader->load(scene));

    auto view = registry->query<RenderObject>();
    EXPECT_EQ(view.size(), 1);

    for(auto [id, archetype, renderObj] : view){
        // Invalid mesh should have invalid handle or valid handle with invalid mesh
        if(renderObj.mesh.isValid()){
            const Mesh* mesh = meshManager->get(renderObj.mesh);
            EXPECT_FALSE(mesh->isValid());
        }
    }
}

TEST_F(SceneLoaderTest, ClearScene){
    SceneDescriptor scene;

    // Load a scene
    {
        EntityDescriptor entity;
        entity.name = "Entity1";
        entity.transform = TransformDescriptor{};
        scene.entities.push_back(entity);
    }

    loader->load(scene);
    EXPECT_EQ(registry->query<Transform>().size(), 1);

    // Clear the scene
    loader->clear();
    EXPECT_EQ(registry->query<Transform>().size(), 0);
}
