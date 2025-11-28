#include "Importer/MeshImporter.hpp"
#include <cmath>
#include <numbers>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace RenderToy;

namespace
{
    // ========================================================================
    // Embedded Mesh Generators
    // ========================================================================

    MeshData createEmbeddedCube(){
        MeshData mesh;

        // Create single submesh
        SubmeshDescriptor submesh;
        submesh.primitiveType = PrimitiveType::TriangleList;
        submesh.materialSlotName = "default";

        // Cube vertices (24 vertices, 4 per face for proper normals/UVs)
        submesh.vertices = {
            // Front face (Z-)
            Vertex{Vec3{-0.5f, -0.5f, -0.5f}, Vec3{0.0f, 0.0f, -1.0f}, Vec2{0.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{0.0f, 0.0f, -1.0f}, Vec2{1.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{0.0f, 0.0f, -1.0f}, Vec2{1.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{-0.5f,  0.5f, -0.5f}, Vec3{0.0f, 0.0f, -1.0f}, Vec2{0.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},

            // Back face (Z+)
            Vertex{Vec3{-0.5f, -0.5f,  0.5f}, Vec3{0.0f, 0.0f, 1.0f}, Vec2{0.0f, 1.0f}, Vec4{-1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{0.0f, 0.0f, 1.0f}, Vec2{1.0f, 1.0f}, Vec4{-1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{0.0f, 0.0f, 1.0f}, Vec2{1.0f, 0.0f}, Vec4{-1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{-0.5f,  0.5f,  0.5f}, Vec3{0.0f, 0.0f, 1.0f}, Vec2{0.0f, 0.0f}, Vec4{-1.0f, 0.0f, 0.0f, 1.0f}},

            // Left face (X-)
            Vertex{Vec3{-0.5f,  0.5f, -0.5f}, Vec3{-1.0f, 0.0f, 0.0f}, Vec2{0.0f, 1.0f}, Vec4{0.0f, 0.0f, -1.0f, 1.0f}},
            Vertex{Vec3{-0.5f, -0.5f, -0.5f}, Vec3{-1.0f, 0.0f, 0.0f}, Vec2{1.0f, 1.0f}, Vec4{0.0f, 0.0f, -1.0f, 1.0f}},
            Vertex{Vec3{-0.5f, -0.5f,  0.5f}, Vec3{-1.0f, 0.0f, 0.0f}, Vec2{1.0f, 0.0f}, Vec4{0.0f, 0.0f, -1.0f, 1.0f}},
            Vertex{Vec3{-0.5f,  0.5f,  0.5f}, Vec3{-1.0f, 0.0f, 0.0f}, Vec2{0.0f, 0.0f}, Vec4{0.0f, 0.0f, -1.0f, 1.0f}},

            // Right face (X+)
            Vertex{Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{1.0f, 0.0f, 0.0f}, Vec2{0.0f, 1.0f}, Vec4{0.0f, 0.0f, 1.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{1.0f, 0.0f, 0.0f}, Vec2{1.0f, 1.0f}, Vec4{0.0f, 0.0f, 1.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{1.0f, 0.0f, 0.0f}, Vec2{1.0f, 0.0f}, Vec4{0.0f, 0.0f, 1.0f, 1.0f}},
            Vertex{Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{1.0f, 0.0f, 0.0f}, Vec2{0.0f, 0.0f}, Vec4{0.0f, 0.0f, 1.0f, 1.0f}},

            // Bottom face (Y-)
            Vertex{Vec3{-0.5f, -0.5f, -0.5f}, Vec3{0.0f, -1.0f, 0.0f}, Vec2{0.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, -0.5f, -0.5f}, Vec3{0.0f, -1.0f, 0.0f}, Vec2{1.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, -0.5f,  0.5f}, Vec3{0.0f, -1.0f, 0.0f}, Vec2{1.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{-0.5f, -0.5f,  0.5f}, Vec3{0.0f, -1.0f, 0.0f}, Vec2{0.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},

            // Top face (Y+)
            Vertex{Vec3{-0.5f,  0.5f, -0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{0.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f,  0.5f, -0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{1.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f,  0.5f,  0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{1.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{-0.5f,  0.5f,  0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{0.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        };

        // Cube indices (12 triangles, 2 per face)
        submesh.indices = {
            // Front
             2,  0,  3,  1,  0,  2,
            // Back
             4,  5,  6,  6,  7,  4,
            // Left
            11,  8,  9,  9, 10, 11,
            // Right
            13, 12, 14, 15, 14, 12,
            // Bottom
            16, 17, 18, 18, 19, 16,
            // Top
            21, 20, 22, 22, 20, 23
        };

        mesh.submeshes.push_back(submesh);

        // Compute AABB
        mesh.bounds.min = Vec3{-0.5f, -0.5f, -0.5f};
        mesh.bounds.max = Vec3{ 0.5f,  0.5f,  0.5f};

        return mesh;
    }

    MeshData createEmbeddedSphere(float radius = 1.0f, int slices = 32, int stacks = 16){
        MeshData mesh;

        SubmeshDescriptor submesh;
        submesh.primitiveType = PrimitiveType::TriangleList;
        submesh.materialSlotName = "default";

        const float dTheta = 2.0f * std::numbers::pi_v<float> / slices;
        const float dPhi = std::numbers::pi_v<float> / stacks;

        // Generate vertices
        for(int i = 0; i <= stacks; ++i){
            const float phi = dPhi * i;
            const float y = radius * std::cos(phi);
            const float rad = radius * std::sin(phi);
            const float v = static_cast<float>(i) / stacks;

            for(int j = 0; j <= slices; ++j){
                const float theta = dTheta * j;
                const float x = rad * std::cos(theta);
                const float z = rad * std::sin(theta);
                const float u = static_cast<float>(j) / slices;

                // Normal is same as position for unit sphere
                Vec3 pos{x, y, z};
                Vec3 normal = pos;

                // Simple tangent calculation
                Vec3 tangent{-std::sin(theta), 0.0f, std::cos(theta)};

                submesh.vertices.push_back(Vertex{
                    pos,
                    normal,
                    Vec2{u, v},
                    Vec4{tangent.x, tangent.y, tangent.z, 1.0f}
                });
            }
        }

        // Generate indices
        for(int i = 0; i < stacks; ++i){
            const uint32_t base = (slices + 1) * i;

            for(int j = 0; j < slices; ++j){
                const uint32_t topLeft = base + j;
                const uint32_t topRight = base + (j + 1);
                const uint32_t bottomLeft = base + (slices + 1) + j;
                const uint32_t bottomRight = base + (slices + 1) + (j + 1);

                submesh.indices.push_back(topLeft);
                submesh.indices.push_back(topRight);
                submesh.indices.push_back(bottomRight);

                submesh.indices.push_back(topLeft);
                submesh.indices.push_back(bottomRight);
                submesh.indices.push_back(bottomLeft);
            }
        }

        mesh.submeshes.push_back(submesh);

        // Compute AABB
        mesh.bounds.min = Vec3{-radius, -radius, -radius};
        mesh.bounds.max = Vec3{ radius,  radius,  radius};

        return mesh;
    }

    MeshData createEmbeddedPlane(){
        MeshData mesh;

        SubmeshDescriptor submesh;
        submesh.primitiveType = PrimitiveType::TriangleList;
        submesh.materialSlotName = "default";

        // Plane in XZ plane (Y=0), 1x1 unit
        submesh.vertices = {
            Vertex{Vec3{-0.5f, 0.0f, -0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{0.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, 0.0f, -0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{1.0f, 1.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{ 0.5f, 0.0f,  0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{1.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{Vec3{-0.5f, 0.0f,  0.5f}, Vec3{0.0f, 1.0f, 0.0f}, Vec2{0.0f, 0.0f}, Vec4{1.0f, 0.0f, 0.0f, 1.0f}},
        };

        submesh.indices = {
            0, 2, 1,
            0, 3, 2
        };

        mesh.submeshes.push_back(submesh);

        // Compute AABB
        mesh.bounds.min = Vec3{-0.5f, 0.0f, -0.5f};
        mesh.bounds.max = Vec3{ 0.5f, 0.0f,  0.5f};

        return mesh;
    }
}

namespace RenderToy
{
    std::optional<MeshData> importMesh(const std::string& filePath){
        Assimp::Importer importer;

        // Configure post-processing flags
        unsigned int flags =
            aiProcess_Triangulate |              // Convert all primitives to triangles
            aiProcess_CalcTangentSpace |         // Calculate tangent vectors
            aiProcess_GenSmoothNormals |         // Generate smooth normals if missing
            aiProcess_JoinIdenticalVertices |    // Merge duplicate vertices
            aiProcess_ImproveCacheLocality |     // Reorder for better cache performance
            aiProcess_SortByPType |              // Split by primitive type
            aiProcess_FindInvalidData |          // Remove invalid data
            aiProcess_ValidateDataStructure;     // Validate the loaded data

        // Load the mesh file
        const aiScene* scene = importer.ReadFile(filePath, flags);

        // Check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            return std::nullopt;
        }

        // We only support meshes with at least one mesh
        if(scene->mNumMeshes == 0){
            return std::nullopt;
        }

        MeshData meshData;
        Vec3 globalMin{
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        };
        Vec3 globalMax{
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
        };
        MeshData meshData;
        Vec3 globalMin{std::numeric_limits<float>::max()};
        Vec3 globalMax{std::numeric_limits<float>::lowest()};

        // Process all meshes in the scene
        for(unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx){
            const aiMesh* aiMesh = scene->mMeshes[meshIdx];

            // Skip non-triangle meshes (should not happen with aiProcess_Triangulate)
            if(aiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE){
                continue;
            }

            SubmeshDescriptor submesh;
            submesh.primitiveType = PrimitiveType::TriangleList;

            // Set material slot name
            if(aiMesh->mMaterialIndex < scene->mNumMaterials){
                aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
                aiString materialName;
                if(material->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS){
                    submesh.materialSlotName = std::string(materialName.C_Str());
                }
                else {
                    submesh.materialSlotName = "material_" + std::to_string(aiMesh->mMaterialIndex);
                }
            }
            else {
                submesh.materialSlotName = "default";
            }

            // Extract vertices
            for(unsigned int i = 0; i < aiMesh->mNumVertices; ++i){
                Vertex vertex;

                // Position
                vertex.position.x = aiMesh->mVertices[i].x;
                vertex.position.y = aiMesh->mVertices[i].y;
                vertex.position.z = aiMesh->mVertices[i].z;

                // Update global AABB
                globalMin.x = std::min(globalMin.x, vertex.position.x);
                globalMin.y = std::min(globalMin.y, vertex.position.y);
                globalMin.z = std::min(globalMin.z, vertex.position.z);
                globalMax.x = std::max(globalMax.x, vertex.position.x);
                globalMax.y = std::max(globalMax.y, vertex.position.y);
                globalMax.z = std::max(globalMax.z, vertex.position.z);

                // Normal
                if(aiMesh->HasNormals()){
                    vertex.normal.x = aiMesh->mNormals[i].x;
                    vertex.normal.y = aiMesh->mNormals[i].y;
                    vertex.normal.z = aiMesh->mNormals[i].z;
                }
                else {
                    vertex.normal = Vec3{0.0f, 1.0f, 0.0f};
                }

                // Texture coordinates (use first UV channel)
                if(aiMesh->HasTextureCoords(0)){
                    vertex.texCoord.x = aiMesh->mTextureCoords[0][i].x;
                    vertex.texCoord.y = aiMesh->mTextureCoords[0][i].y;
                }
                else {
                    vertex.texCoord = Vec2{0.0f, 0.0f};
                }

                // Tangent
                if(aiMesh->HasTangentsAndBitangents()){
                    vertex.tangent.x = aiMesh->mTangents[i].x;
                    vertex.tangent.y = aiMesh->mTangents[i].y;
                    vertex.tangent.z = aiMesh->mTangents[i].z;
                    vertex.tangent.w = 1.0f; // Handedness (always 1 for now)
                } else {
                    vertex.tangent = Vec4{1.0f, 0.0f, 0.0f, 1.0f};
                }

                submesh.vertices.push_back(vertex);
            }

            // Extract indices
            for(unsigned int i = 0; i < aiMesh->mNumFaces; ++i){
                const aiFace& face = aiMesh->mFaces[i];

                // Should always be 3 due to aiProcess_Triangulate
                if(face.mNumIndices == 3){
                    submesh.indices.push_back(face.mIndices[0]);
                    submesh.indices.push_back(face.mIndices[1]);
                    submesh.indices.push_back(face.mIndices[2]);
                }
            }

            meshData.submeshes.push_back(submesh);
        }

        // Set global AABB
        meshData.bounds.min = globalMin;
        meshData.bounds.max = globalMax;

        return meshData;
    }

    std::optional<MeshData> loadEmbeddedMesh(const std::string& name){
        if(name == "cube")
            return createEmbeddedCube();
        else if(name == "sphere")
            return createEmbeddedSphere();
        else if(name == "plane")
            return createEmbeddedPlane();
        else
            return std::nullopt;
    }
}
