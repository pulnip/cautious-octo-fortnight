#include <gtest/gtest.h>
#include "Importer/SceneImporter.hpp"
#include "Content/SceneFormat.hpp"
#include <filesystem>

using namespace RenderToy;
namespace fs = std::filesystem;

// Helper to get test data directory
static std::string getTestDataPath(const char* filename){
    // Assuming tests run from project root
    return "engine/test/Importer/test_data/" + std::string(filename);
}

// ============================================================================
// Basic Parsing Tests
// ============================================================================

TEST(SceneImporter, ImportEmptyScene){
    auto result = importScene(getTestDataPath("empty.scene.toml"));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->entities.size(), 0);
}

TEST(SceneImporter, ImportSimpleTransform){
    auto result = importScene(getTestDataPath("simple_transform.scene.toml"));

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->entities.size(), 1);

    const auto& entity = result->entities[0];
    EXPECT_EQ(entity.name, "Box");
    ASSERT_TRUE(entity.hasTransform());

    const auto& transform = *entity.transform;
    EXPECT_EQ(transform.position, (Vec3{1.0f, 2.0f, 3.0f}));
    EXPECT_EQ(transform.rotation, unitQuat());
    EXPECT_EQ(transform.scale, ones());
}

TEST(SceneImporter, ImportSimpleMesh){
    auto result = importScene(getTestDataPath("simple_mesh.scene.toml"));

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->entities.size(), 1);

    const auto& entity = result->entities[0];
    EXPECT_EQ(entity.name, "Cube");
    ASSERT_TRUE(entity.hasMesh());

    const auto& mesh = *entity.mesh;
    EXPECT_EQ(mesh.uri, "embedded:cube");
}

// ============================================================================
// Complex Scene Tests
// ============================================================================

TEST(SceneImporter, ImportComplexScene){
    auto result = importScene(getTestDataPath("complex.scene.toml"));

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->entities.size(), 3);

    // Check Player entity
    const auto& player = result->entities[0];
    EXPECT_EQ(player.name, "Player");
    ASSERT_TRUE(player.hasTransform());
    ASSERT_TRUE(player.hasMesh());
    ASSERT_TRUE(player.hasRigidbody());
    ASSERT_TRUE(player.hasBoxCollider());

    // Transform
    EXPECT_EQ(player.transform->position, (Vec3{0.0f, 1.0f, 0.0f}));

    // Mesh
    EXPECT_EQ(player.mesh->uri, "file:asset/player.rtmesh");
    EXPECT_FALSE(player.mesh->materialOverrides.empty());
    EXPECT_EQ(player.mesh->materialOverrides[0].baseColorTexture, "file:asset/player_diffuse.png");
    EXPECT_EQ(player.mesh->materialOverrides[0].targetSlot, "body");
    EXPECT_EQ(player.mesh->shader.modulePath, "");
    EXPECT_EQ(player.mesh->shader.vertexFunction, "vs_main");
    EXPECT_EQ(player.mesh->shader.fragmentFunction, "fs_pbr");

    // Rigidbody
    EXPECT_EQ(player.rigidbody->velocity, zeros());
    EXPECT_TRUE(player.rigidbody->useGravity);
    EXPECT_FLOAT_EQ(player.rigidbody->mass, 70.0f);

    // BoxCollider
    EXPECT_EQ(player.boxCollider->center, (Vec3{0.0f, 0.9f, 0.0f}));
    EXPECT_EQ(player.boxCollider->size, (Vec3{0.6f, 1.8f, 0.6f}));
    EXPECT_FLOAT_EQ(player.boxCollider->material.friction, 0.5f);
    EXPECT_FLOAT_EQ(player.boxCollider->material.bounciness, 0.0f);

    // Check Ground entity
    const auto& ground = result->entities[1];
    EXPECT_EQ(ground.name, "Ground");
    ASSERT_TRUE(ground.hasTransform());
    ASSERT_TRUE(ground.hasMesh());
    EXPECT_FALSE(ground.hasRigidbody());

    // Check Camera entity
    const auto& camera = result->entities[2];
    EXPECT_EQ(camera.name, "MainCamera");
    ASSERT_TRUE(camera.hasTransform());
    ASSERT_TRUE(camera.hasCamera());

    EXPECT_EQ(camera.camera->projection, ProjectionType::Perspective);
    EXPECT_FLOAT_EQ(camera.camera->fov, 60.0f);
    EXPECT_FLOAT_EQ(camera.camera->nearPlane, 0.1f);
    EXPECT_FLOAT_EQ(camera.camera->farPlane, 1000.0f);
}

// ============================================================================
// Default Values Tests
// ============================================================================

TEST(SceneImporter, TransformDefaultValues){
    std::string tomlText = R"(
        [[entities]]
        name = "DefaultEntity"
        [entities.transform]
    )";

    auto result = importSceneFromString(tomlText);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->entities.size(), 1);

    const auto& entity = result->entities[0];
    ASSERT_TRUE(entity.hasTransform());

    // Should use default values
    EXPECT_EQ(entity.transform->position, zeros());
    EXPECT_EQ(entity.transform->rotation, unitQuat());
    EXPECT_EQ(entity.transform->scale, ones());
}

TEST(SceneImporter, MeshDefaultShader){
    std::string tomlText = R"(
        [[entities]]
        name = "SimpleMesh"
        [entities.mesh]
        uri = "test.rtmesh"
    )";

    auto result = importSceneFromString(tomlText);

    ASSERT_TRUE(result.has_value());
    const auto& entity = result->entities[0];
    ASSERT_TRUE(entity.hasMesh());

    // Shader should have default values if not specified
    EXPECT_EQ(entity.mesh->shader.modulePath, "");
    EXPECT_EQ(entity.mesh->shader.vertexFunction, "vs_main");
    EXPECT_EQ(entity.mesh->shader.fragmentFunction, "fs_main");
}

// ============================================================================
// Multiple Entities Tests
// ============================================================================

TEST(SceneImporter, MultipleEntities){
    std::string tomlText = R"(
        [[entities]]
        name = "Entity1"
        [entities.transform]
        position = [1.0, 0.0, 0.0]

        [[entities]]
        name = "Entity2"
        [entities.transform]
        position = [2.0, 0.0, 0.0]

        [[entities]]
        name = "Entity3"
        [entities.transform]
        position = [3.0, 0.0, 0.0]
    )";

    auto result = importSceneFromString(tomlText);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->entities.size(), 3);

    EXPECT_EQ(result->entities[0].name, "Entity1");
    EXPECT_EQ(result->entities[0].transform->position, (Vec3{1.0f, 0.0f, 0.0f}));

    EXPECT_EQ(result->entities[1].name, "Entity2");
    EXPECT_EQ(result->entities[1].transform->position, (Vec3{2.0f, 0.0f, 0.0f}));

    EXPECT_EQ(result->entities[2].name, "Entity3");
    EXPECT_EQ(result->entities[2].transform->position, (Vec3{3.0f, 0.0f, 0.0f}));
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST(SceneImporter, InvalidFilePath){
    auto result = importScene("nonexistent/path/to/scene.toml");

    EXPECT_FALSE(result.has_value());
}

TEST(SceneImporter, InvalidVectorLength){
    std::string tomlText = R"(
        [[entities]]
        name = "BadEntity"
        [entities.transform]
        position = [1.0, 2.0]  # Should be 3 elements
    )";

    auto result = importSceneFromString(tomlText);

    // Should return error (nullopt) for invalid data
    EXPECT_FALSE(result.has_value());
}

TEST(SceneImporter, InvalidFieldType){
    std::string tomlText = R"(
        [[entities]]
        name = "BadEntity"
        [entities.mesh]
        uri = 123  # Should be string
    )";

    auto result = importSceneFromString(tomlText);

    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Component Combinations Tests
// ============================================================================

TEST(SceneImporter, EntityWithMultipleComponents){
    std::string tomlText = R"(
        [[entities]]
        name = "ComplexEntity"
        [entities.transform]
        position = [5.0, 10.0, 15.0]
        [entities.mesh]
        uri = "test.rtmesh"
        [entities.rigidbody]
        mass = 2.5
        useGravity = false
    )";

    auto result = importSceneFromString(tomlText);

    ASSERT_TRUE(result.has_value());
    const auto& entity = result->entities[0];

    EXPECT_TRUE(entity.hasTransform());
    EXPECT_TRUE(entity.hasMesh());
    EXPECT_TRUE(entity.hasRigidbody());
    EXPECT_FALSE(entity.hasCamera());
    EXPECT_FALSE(entity.hasBoxCollider());
    EXPECT_FALSE(entity.hasSphereCollider());
}

TEST(SceneImporter, EntityWithNoComponents){
    std::string tomlText = R"(
        [[entities]]
        name = "EmptyEntity"
    )";

    auto result = importSceneFromString(tomlText);

    ASSERT_TRUE(result.has_value());
    const auto& entity = result->entities[0];

    EXPECT_EQ(entity.name, "EmptyEntity");
    EXPECT_FALSE(entity.hasTransform());
    EXPECT_FALSE(entity.hasMesh());
    EXPECT_FALSE(entity.hasRigidbody());
}

// ============================================================================
// SphereCollider Tests
// ============================================================================

TEST(SceneImporter, SphereCollider){
    std::string tomlText = R"(
        [[entities]]
        name = "Ball"
        [entities.sphereCollider]
        center = [0.0, 1.0, 0.0]
        radius = 0.5
            [entities.sphereCollider.material]
            friction = 0.3
            bounciness = 0.8
    )";

    auto result = importSceneFromString(tomlText);

    ASSERT_TRUE(result.has_value());
    const auto& entity = result->entities[0];
    ASSERT_TRUE(entity.hasSphereCollider());

    const auto& collider = *entity.sphereCollider;
    EXPECT_EQ(collider.center, (Vec3{0.0f, 1.0f, 0.0f}));
    EXPECT_FLOAT_EQ(collider.radius, 0.5f);
    EXPECT_FLOAT_EQ(collider.material.friction, 0.3f);
    EXPECT_FLOAT_EQ(collider.material.bounciness, 0.8f);
}
