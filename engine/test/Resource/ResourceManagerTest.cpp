#include <gtest/gtest.h>
#include "Resource/ResourceManager.hpp"
#include "Resource/SubmeshTraits.hpp"
#include "RHI/RHIDevice.hpp"
#include <memory>

using RenderToy::ResourceManager;

namespace{
    struct MockResource{ int i; };
    struct MockResourceRequest{};
    struct MockResourceKey{
        auto operator<=>(const MockResourceKey&) const = default;
    };
    struct MockResourceKeyHash{
        inline size_t operator()(const MockResourceKey& key) const noexcept{
            return 0;
        }
    };
}

template<>
struct RenderToy::ResourceTraits<MockResource>{
    using Request = MockResourceRequest;
    using Key     = MockResourceKey;
    using KeyHash = MockResourceKeyHash;

    inline static Key makeKey(const Request& request){
        return Key{};
    }

    inline static MockResource load(const Request& request){
        return MockResource{};
    }
};

TEST(ResourceManager, Trivial){
    ResourceManager<MockResource> manager;
    MockResourceRequest request;

    auto handle1 = manager.getOrLoad(request);
    auto handle2 = manager.getOrLoad(request);

    EXPECT_EQ(handle1, handle2);
}

// ============================================================================
// Submesh ResourceManager Tests
// ============================================================================

class SubmeshResourceManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<RenderToy::RHIDevice> device;

    void SetUp() override {
        // Create RHI device for testing
        RHIDeviceCreateDesc desc{
            .enableDebugLayer = false,
            .enableGPUValidation = false,
            .enableValidation = false,
            .applicationName = "SubmeshResourceManagerTest",
            .windowHandle = nullptr
        };
        device = RenderToy::RHIDevice::create(desc);
        ASSERT_NE(device, nullptr) << "Failed to create RHI device";
    }

    void TearDown() override {
        device.reset();
    }
};

TEST_F(SubmeshResourceManagerTest, LoadMeshFromOBJ){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/simple_triangle.obj",
        .device = device.get()
    };

    auto handle = meshManager.getOrLoad(request);
    ASSERT_TRUE(handle.isValid());

    const RenderToy::Submesh* mesh = meshManager.get(handle);
    ASSERT_NE(mesh, nullptr);
    EXPECT_TRUE(mesh->isValid());
    EXPECT_EQ(mesh->vertexCount, 3);
    EXPECT_EQ(mesh->indexCount, 3);
    EXPECT_TRUE(mesh->hasIndices());
}

TEST_F(SubmeshResourceManagerTest, LoadMeshFromOBJ_Cube){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/simple_cube.obj",
        .device = device.get()
    };

    auto handle = meshManager.getOrLoad(request);
    ASSERT_TRUE(handle.isValid());

    const RenderToy::Submesh* mesh = meshManager.get(handle);
    ASSERT_NE(mesh, nullptr);
    EXPECT_TRUE(mesh->isValid());
    EXPECT_GT(mesh->vertexCount, 0);
    EXPECT_EQ(mesh->indexCount, 36);  // 12 triangles * 3 indices
}

TEST_F(SubmeshResourceManagerTest, CachingWorks){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/simple_triangle.obj",
        .device = device.get()
    };

    auto handle1 = meshManager.getOrLoad(request);
    auto handle2 = meshManager.getOrLoad(request);

    // Should return same handle (cached)
    EXPECT_EQ(handle1, handle2);
}

TEST_F(SubmeshResourceManagerTest, UnloadMesh){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/simple_triangle.obj",
        .device = device.get()
    };

    auto handle = meshManager.getOrLoad(request);
    ASSERT_TRUE(handle.isValid());

    // Unload the mesh
    meshManager.unload(handle);

    // Loading again should create a new resource
    auto handle2 = meshManager.getOrLoad(request);
    EXPECT_NE(handle, handle2);
}

TEST_F(SubmeshResourceManagerTest, LoadNonexistentFile){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "nonexistent_mesh.obj",
        .device = device.get()
    };

    auto handle = meshManager.getOrLoad(request);

    // Should return valid handle but invalid mesh
    ASSERT_TRUE(handle.isValid());
    const RenderToy::Submesh* mesh = meshManager.get(handle);
    EXPECT_FALSE(mesh->isValid());
}

TEST_F(SubmeshResourceManagerTest, LoadNullDevice){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/simple_triangle.obj",
        .device = nullptr  // Invalid device
    };

    auto handle = meshManager.getOrLoad(request);

    // Should fail gracefully
    ASSERT_TRUE(handle.isValid());
    const RenderToy::Submesh* mesh = meshManager.get(handle);
    EXPECT_FALSE(mesh->isValid());
}

TEST_F(SubmeshResourceManagerTest, LoadUnsupportedFormat){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/empty.scene.toml",  // Not a mesh file
        .device = device.get()
    };

    auto handle = meshManager.getOrLoad(request);

    // Should fail gracefully
    ASSERT_TRUE(handle.isValid());
    const RenderToy::Submesh* mesh = meshManager.get(handle);
    EXPECT_FALSE(mesh->isValid());
}

TEST_F(SubmeshResourceManagerTest, GPUBuffersAreValid){
    ResourceManager<RenderToy::Submesh> meshManager;

    RenderToy::SubmeshRequest request{
        .path = "engine/test/Importer/test_data/simple_triangle.obj",
        .device = device.get()
    };

    auto handle = meshManager.getOrLoad(request);
    ASSERT_TRUE(handle.isValid());

    const RenderToy::Submesh* mesh = meshManager.get(handle);
    ASSERT_NE(mesh, nullptr);

    // Check GPU buffers are valid
    EXPECT_TRUE(mesh->vertexBuffer.isValid());
    EXPECT_TRUE(mesh->indexBuffer.isValid());
}