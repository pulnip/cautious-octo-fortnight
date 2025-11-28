#include <gtest/gtest.h>
#include "Content/MeshFormat.hpp"

using namespace RenderToy;

// ============================================================================
// Vertex Tests
// ============================================================================

TEST(Vertex, SizeAndAlignment){
    static_assert(sizeof(Vertex)==48);
    static_assert(std::is_trivially_copyable_v<Vertex>);
}

TEST(Vertex, Construction){
    Vertex v{
        .position = Vec3{1.0f, 2.0f, 3.0f},
        .normal = Vec3{0.0f, 1.0f, 0.0f},
        .texCoord = Vec2{0.5f, 0.5f},
        .tangent = Vec4{1.0f, 0.0f, 0.0f, 1.0f}
    };

    EXPECT_EQ(v.position, (Vec3{1.0f, 2.0f, 3.0f}));
    EXPECT_EQ(v.normal, (Vec3{0.0f, 1.0f, 0.0f}));
    EXPECT_EQ(v.texCoord, (Vec2{0.5f, 0.5f}));
}

// ============================================================================
// AABB Tests
// ============================================================================

TEST(MeshFormat, DefaultIsPoint){
    AABB box;
    // Default AABB (min = max = 0) is technically valid (represents a point)
    EXPECT_TRUE(box.isValid());
    EXPECT_EQ(box.center(), zeros());
    EXPECT_EQ(box.extents(), zeros());
}

TEST(MeshFormat, ValidBox){
    AABB box{
        .min = Vec3{-1.0f, -1.0f, -1.0f},
        .max = Vec3{1.0f, 1.0f, 1.0f}
    };

    EXPECT_TRUE(box.isValid());
    EXPECT_EQ(box.center(), (Vec3{0.0f, 0.0f, 0.0f}));
    EXPECT_EQ(box.extents(), (Vec3{1.0f, 1.0f, 1.0f}));
}

// ============================================================================
// SubmeshDescriptor Tests
// ============================================================================

TEST(MeshFormat, EmptySubmesh){
    SubmeshDescriptor submesh;

    EXPECT_EQ(submesh.vertexCount(), 0);
    EXPECT_EQ(submesh.indexCount(), 0);
    EXPECT_EQ(submesh.triangleCount(), 0);
    EXPECT_EQ(submesh.primitiveType, PrimitiveType::TriangleList);
}

TEST(MeshFormat, TriangleCounting){
    SubmeshDescriptor submesh{
        .vertices = {
            Vertex{.position = Vec3{0.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{1.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{0.0f, 1.0f, 0.0f}}
        },
        .indices = {0, 1, 2},
        .primitiveType = PrimitiveType::TriangleList
    };

    EXPECT_EQ(submesh.vertexCount(), 3);
    EXPECT_EQ(submesh.indexCount(), 3);
    EXPECT_EQ(submesh.triangleCount(), 1);
}

// ============================================================================
// MeshData Tests
// ============================================================================

TEST(MeshFormat, MeshData_Empty){
    MeshData mesh;

    EXPECT_EQ(mesh.submeshCount(), 0);
    EXPECT_EQ(mesh.materialCount(), 0);
    EXPECT_EQ(mesh.totalVertexCount(), 0);
    EXPECT_EQ(mesh.totalIndexCount(), 0);
    EXPECT_FALSE(mesh.isValid());  // Empty mesh is invalid
}

TEST(MeshFormat, MeshData_SingleSubmesh){
    MeshData mesh;
    mesh.submeshes.push_back(SubmeshDescriptor{
        .vertices = {
            Vertex{.position = Vec3{0.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{1.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{0.0f, 1.0f, 0.0f}}
        },
        .indices = {0, 1, 2},
        .materialSlotName = "default"
    });

    mesh.bounds = AABB{
        .min = Vec3{0.0f, 0.0f, 0.0f},
        .max = Vec3{1.0f, 1.0f, 0.0f}
    };

    EXPECT_EQ(mesh.submeshCount(), 1);
    EXPECT_EQ(mesh.totalVertexCount(), 3);
    EXPECT_EQ(mesh.totalIndexCount(), 3);
    EXPECT_TRUE(mesh.isValid());
}

TEST(MeshFormat, MeshData_FindMaterial){
    MeshData mesh;
    mesh.materials["default"] = MaterialDescriptor{
        .name = "DefaultMaterial",
        .type = MaterialType::PBR
    };

    auto* mat = mesh.findMaterial("default");
    ASSERT_NE(mat, nullptr);
    EXPECT_EQ(mat->name, "DefaultMaterial");

    auto* notFound = mesh.findMaterial("nonexistent");
    EXPECT_EQ(notFound, nullptr);
}

// ============================================================================
// Serialization Tests
// ============================================================================

TEST(MeshFormat, Serialize_EmptyMesh){
    MeshData mesh;
    mesh.bounds = AABB{
        .min = Vec3{0.0f, 0.0f, 0.0f},
        .max = Vec3{0.0f, 0.0f, 0.0f}
    };
    mesh.submeshes.push_back(SubmeshDescriptor{});  // Need at least one submesh

    auto bytes = serializeMesh(mesh);

    // Should at least have header
    EXPECT_GE(bytes.size(), sizeof(MeshFileHeader));

    // Check magic number
    EXPECT_EQ(std::memcmp(bytes.data(), MESH_FILE_MAGIC, 8), 0);
}

TEST(MeshFormat, Serialize_Deserialize_Roundtrip){
    // Create test mesh
    MeshData original;

    // Add single triangle
    original.submeshes.push_back(SubmeshDescriptor{
        .vertices = {
            Vertex{
                .position = Vec3{0.0f, 0.0f, 0.0f},
                .normal = Vec3{0.0f, 0.0f, 1.0f},
                .texCoord = Vec2{0.0f, 0.0f},
                .tangent = Vec4{1.0f, 0.0f, 0.0f, 1.0f}
            },
            Vertex{
                .position = Vec3{1.0f, 0.0f, 0.0f},
                .normal = Vec3{0.0f, 0.0f, 1.0f},
                .texCoord = Vec2{1.0f, 0.0f},
                .tangent = Vec4{1.0f, 0.0f, 0.0f, 1.0f}
            },
            Vertex{
                .position = Vec3{0.0f, 1.0f, 0.0f},
                .normal = Vec3{0.0f, 0.0f, 1.0f},
                .texCoord = Vec2{0.0f, 1.0f},
                .tangent = Vec4{1.0f, 0.0f, 0.0f, 1.0f}
            }
        },
        .indices = {0, 1, 2},
        .primitiveType = PrimitiveType::TriangleList,
        .materialSlotName = "default"
    });

    original.bounds = AABB{
        .min = Vec3{0.0f, 0.0f, 0.0f},
        .max = Vec3{1.0f, 1.0f, 0.0f}
    };

    // Serialize
    auto bytes = serializeMesh(original);
    ASSERT_GT(bytes.size(), 0);

    // Deserialize
    auto loaded = deserializeMesh(bytes);
    ASSERT_TRUE(loaded.has_value());

    // Verify
    EXPECT_EQ(loaded->submeshCount(), original.submeshCount());
    EXPECT_EQ(loaded->totalVertexCount(), original.totalVertexCount());
    EXPECT_EQ(loaded->totalIndexCount(), original.totalIndexCount());

    ASSERT_EQ(loaded->submeshes.size(), 1);
    EXPECT_EQ(loaded->submeshes[0].vertices.size(), 3);
    EXPECT_EQ(loaded->submeshes[0].indices.size(), 3);

    // Check vertices match
    for(size_t i = 0; i < 3; ++i){
        EXPECT_EQ(loaded->submeshes[0].vertices[i].position,
                  original.submeshes[0].vertices[i].position);
        EXPECT_EQ(loaded->submeshes[0].vertices[i].normal,
                  original.submeshes[0].vertices[i].normal);
    }

    // Check indices match
    for(size_t i = 0; i < 3; ++i){
        EXPECT_EQ(loaded->submeshes[0].indices[i],
                  original.submeshes[0].indices[i]);
    }

    // Check bounds
    EXPECT_EQ(loaded->bounds.min, original.bounds.min);
    EXPECT_EQ(loaded->bounds.max, original.bounds.max);
}

TEST(MeshFormat, Deserialize_InvalidMagicNumber){
    std::vector<uint8_t> badData(100, 0xFF);

    auto result = deserializeMesh(badData);
    EXPECT_FALSE(result.has_value());
}

TEST(MeshFormat, Deserialize_TooSmall){
    std::vector<uint8_t> tooSmall(10, 0x00);

    auto result = deserializeMesh(tooSmall);
    EXPECT_FALSE(result.has_value());
}

TEST(MeshFormat, Serialize_MultipleSubmeshes){
    MeshData mesh;

    // First submesh (triangle)
    mesh.submeshes.push_back(SubmeshDescriptor{
        .vertices = {
            Vertex{.position = Vec3{0.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{1.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{0.0f, 1.0f, 0.0f}}
        },
        .indices = {0, 1, 2},
        .materialSlotName = "material1"
    });

    // Second submesh (quad)
    mesh.submeshes.push_back(SubmeshDescriptor{
        .vertices = {
            Vertex{.position = Vec3{2.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{3.0f, 0.0f, 0.0f}},
            Vertex{.position = Vec3{3.0f, 1.0f, 0.0f}},
            Vertex{.position = Vec3{2.0f, 1.0f, 0.0f}}
        },
        .indices = {0, 1, 2, 2, 3, 0},
        .materialSlotName = "material2"
    });

    mesh.bounds = AABB{
        .min = Vec3{0.0f, 0.0f, 0.0f},
        .max = Vec3{3.0f, 1.0f, 0.0f}
    };

    // Serialize and deserialize
    auto bytes = serializeMesh(mesh);
    auto loaded = deserializeMesh(bytes);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->submeshCount(), 2);
    EXPECT_EQ(loaded->totalVertexCount(), 7);  // 3 + 4
    EXPECT_EQ(loaded->totalIndexCount(), 9);   // 3 + 6
}
