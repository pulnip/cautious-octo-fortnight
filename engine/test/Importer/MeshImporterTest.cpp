#include <gtest/gtest.h>
#include "Importer/MeshImporter.hpp"
#include "Content/MeshFormat.hpp"

using namespace RenderToy;

// ============================================================================
// Embedded Mesh Tests
// ============================================================================

TEST(MeshImporter, LoadEmbeddedCube){
    auto result = loadEmbeddedMesh("cube");

    ASSERT_TRUE(result.has_value());
    const auto& mesh = *result;

    // Cube should have geometry
    EXPECT_GT(mesh.totalVertexCount(), 0);
    EXPECT_GT(mesh.totalIndexCount(), 0);

    // Should have at least one submesh
    ASSERT_GE(mesh.submeshCount(), 1);

    // Check first submesh
    const auto& submesh = mesh.submeshes[0];
    EXPECT_EQ(submesh.primitiveType, PrimitiveType::TriangleList);
    EXPECT_GT(submesh.vertexCount(), 0);
    EXPECT_GT(submesh.indexCount(), 0);

    // Indices should be multiple of 3 (triangles)
    EXPECT_EQ(submesh.indexCount() % 3, 0);

    // AABB should be valid
    EXPECT_TRUE(mesh.bounds.isValid());
}

TEST(MeshImporter, LoadEmbeddedSphere){
    auto result = loadEmbeddedMesh("sphere");

    ASSERT_TRUE(result.has_value());
    const auto& mesh = *result;

    EXPECT_GT(mesh.totalVertexCount(), 0);
    EXPECT_GT(mesh.totalIndexCount(), 0);
    EXPECT_GE(mesh.submeshCount(), 1);

    // AABB should be roughly centered around origin
    const auto center = mesh.bounds.center();
    EXPECT_NEAR(center.x, 0.0f, 0.01f);
    EXPECT_NEAR(center.y, 0.0f, 0.01f);
    EXPECT_NEAR(center.z, 0.0f, 0.01f);
}

TEST(MeshImporter, LoadEmbeddedPlane){
    auto result = loadEmbeddedMesh("plane");

    ASSERT_TRUE(result.has_value());
    const auto& mesh = *result;

    EXPECT_GT(mesh.totalVertexCount(), 0);
    EXPECT_GT(mesh.totalIndexCount(), 0);
    EXPECT_GE(mesh.submeshCount(), 1);
}

TEST(MeshImporter, LoadEmbeddedInvalidName){
    auto result = loadEmbeddedMesh("nonexistent");

    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// Vertex Attribute Tests
// ============================================================================

TEST(MeshImporter, CubeHasNormals){
    auto result = loadEmbeddedMesh("cube");
    ASSERT_TRUE(result.has_value());

    const auto& submesh = result->submeshes[0];

    // Check that vertices have non-zero normals
    bool hasNonZeroNormals = false;
    for(const auto& v : submesh.vertices){
        if(v.normal.x != 0.0f || v.normal.y != 0.0f || v.normal.z != 0.0f){
            hasNonZeroNormals = true;
            break;
        }
    }

    EXPECT_TRUE(hasNonZeroNormals);
}

TEST(MeshImporter, CubeHasTexCoords){
    auto result = loadEmbeddedMesh("cube");
    ASSERT_TRUE(result.has_value());

    const auto& submesh = result->submeshes[0];

    // Check that vertices have valid tex coords (in [0,1] range typically)
    bool hasTexCoords = false;
    for(const auto& v : submesh.vertices){
        if(v.texCoord.x >= 0.0f && v.texCoord.x <= 1.0f &&
           v.texCoord.y >= 0.0f && v.texCoord.y <= 1.0f){
            hasTexCoords = true;
            break;
        }
    }

    EXPECT_TRUE(hasTexCoords);
}

// ============================================================================
// Mesh Validity Tests
// ============================================================================

TEST(MeshImporter, CubeIndicesInBounds){
    auto result = loadEmbeddedMesh("cube");
    ASSERT_TRUE(result.has_value());

    const auto& submesh = result->submeshes[0];
    const uint32_t vertexCount = submesh.vertexCount();

    // All indices should be < vertexCount
    for(const auto idx : submesh.indices){
        EXPECT_LT(idx, vertexCount);
    }
}

TEST(MeshImporter, CubeTriangleCount){
    auto result = loadEmbeddedMesh("cube");
    ASSERT_TRUE(result.has_value());

    const auto& submesh = result->submeshes[0];

    // Cube has 6 faces, each with 2 triangles = 12 triangles total
    EXPECT_EQ(submesh.triangleCount(), 12);
}

// ============================================================================
// AABB Tests
// ============================================================================

TEST(MeshImporter, CubeAABBValid){
    auto result = loadEmbeddedMesh("cube");
    ASSERT_TRUE(result.has_value());

    const auto& bounds = result->bounds;

    EXPECT_TRUE(bounds.isValid());

    // Cube AABB should contain origin
    EXPECT_LE(bounds.min.x, 0.0f);
    EXPECT_GE(bounds.max.x, 0.0f);
    EXPECT_LE(bounds.min.y, 0.0f);
    EXPECT_GE(bounds.max.y, 0.0f);
    EXPECT_LE(bounds.min.z, 0.0f);
    EXPECT_GE(bounds.max.z, 0.0f);
}

TEST(MeshImporter, SphereAABBRoughlySpherical){
    auto result = loadEmbeddedMesh("sphere");
    ASSERT_TRUE(result.has_value());

    const auto& bounds = result->bounds;
    const auto extents = bounds.extents();

    // Sphere extents should be roughly equal in all dimensions
    EXPECT_NEAR(extents.x, extents.y, 0.1f);
    EXPECT_NEAR(extents.y, extents.z, 0.1f);
    EXPECT_NEAR(extents.z, extents.x, 0.1f);
}

// ============================================================================
// File Import Tests (Assimp)
// ============================================================================

namespace {
    std::string getTestMeshPath(const std::string& filename){
        return "engine/test/Importer/test_data/" + filename;
    }
}

TEST(MeshImporter, ImportSimpleTriangle){
    auto result = importMesh(getTestMeshPath("simple_triangle.obj"));

    ASSERT_TRUE(result.has_value());
    const auto& mesh = *result;

    // Should have geometry
    EXPECT_GT(mesh.totalVertexCount(), 0);
    EXPECT_GT(mesh.totalIndexCount(), 0);
    EXPECT_GE(mesh.submeshCount(), 1);

    // Triangle should have exactly 3 vertices and 3 indices
    const auto& submesh = mesh.submeshes[0];
    EXPECT_EQ(submesh.vertexCount(), 3);
    EXPECT_EQ(submesh.indexCount(), 3);
    EXPECT_EQ(submesh.triangleCount(), 1);
}

TEST(MeshImporter, ImportSimpleCube){
    auto result = importMesh(getTestMeshPath("simple_cube.obj"));

    ASSERT_TRUE(result.has_value());
    const auto& mesh = *result;

    // Should have geometry
    EXPECT_GT(mesh.totalVertexCount(), 0);
    EXPECT_GT(mesh.totalIndexCount(), 0);

    // Cube has 12 triangles (6 faces * 2 triangles) = 36 indices
    EXPECT_EQ(mesh.totalIndexCount(), 36);
}

TEST(MeshImporter, ImportedMeshHasValidBounds){
    auto result = importMesh(getTestMeshPath("simple_cube.obj"));

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->bounds.isValid());
}

TEST(MeshImporter, ImportNonexistentFile){
    auto result = importMesh("nonexistent_file.obj");

    EXPECT_FALSE(result.has_value());
}

TEST(MeshImporter, ImportInvalidFile){
    auto result = importMesh(getTestMeshPath("empty.scene.toml")); // Not a mesh file

    EXPECT_FALSE(result.has_value());
}
