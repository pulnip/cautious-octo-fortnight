#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "Log/Log.hpp"
#include "Resource/ResourceTraits.hpp"
#include "Resource/Mesh.hpp"
#include "Content/MeshFormat.hpp"
#include "Importer/MeshImporter.hpp"
#include "RHI/RHIDevice.hpp"

namespace RenderToy
{
    struct MeshRequest{
        std::filesystem::path path;
        RHIDevice* device = nullptr;  // Required for GPU upload
    };

    struct MeshKey{
        std::string canonicalPath;

        auto operator<=>(const MeshKey&) const = default;
    };

    struct MeshKeyHash{
        inline size_t operator()(const MeshKey& k) const noexcept{
            return std::hash<std::string>{}(k.canonicalPath);
        }
    };

    template<>
    struct ResourceTraits<Mesh>{
        using Request = MeshRequest;
        using Key     = MeshKey;
        using KeyHash = MeshKeyHash;

        inline static Key makeKey(const Request& request){
            auto canonical = std::filesystem::weakly_canonical(request.path);
            return Key{
                .canonicalPath = canonical.string()
            };
        }

        inline static Mesh load(const Request& request){
            if(!request.device){
                LOG_ERROR(LOG_RESOURCE, "MeshRequest.device is null");
                return Mesh{};
            }

            // Determine file type and load accordingly
            auto ext = request.path.extension().string();
            std::optional<MeshData> meshData;

            if(ext == ".rtmesh"){
                // Load from binary format - read file into memory first
                std::ifstream file(request.path, std::ios::binary | std::ios::ate);
                if(!file.is_open()){
                    LOG_ERROR(LOG_RESOURCE, "Failed to open file: {}", request.path.string());
                    return Mesh{};
                }

                auto fileSize = file.tellg();
                file.seekg(0, std::ios::beg);

                std::vector<uint8_t> buffer(fileSize);
                file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
                file.close();

                meshData = deserializeMesh(buffer);
            }
            else if(ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb"){
                // Load from 3D model file
                meshData = importMesh(request.path.string());
            }
            else {
                LOG_ERROR(LOG_RESOURCE, "Unsupported mesh file format: {}", ext);
                return Mesh{};
            }

            if(!meshData.has_value()){
                LOG_ERROR(LOG_RESOURCE, "Failed to load mesh: {}", request.path.string());
                return Mesh{};
            }

            // For now, we only support single submesh
            // TODO: Support multi-submesh meshes
            if(meshData->submeshes.empty()){
                LOG_ERROR(LOG_RESOURCE, "Mesh has no submeshes: {}", request.path.string());
                return Mesh{};
            }

            const auto& submesh = meshData->submeshes[0];

            // Create debug names (persistent strings)
            std::string vertexDebugName = request.path.filename().string() + "_vertices";
            std::string indexDebugName = request.path.filename().string() + "_indices";

            // Create vertex buffer
            RHIBufferCreateDesc vertexBufferDesc{
                .size = submesh.vertices.size() * sizeof(Vertex),
                .usage = BufVertexBuffer,
                .stride = 0,
                .initialData = submesh.vertices.data(),
                .debugName = vertexDebugName.c_str()
            };

            auto vertexBuffer = request.device->createBuffer(vertexBufferDesc);
            if(!vertexBuffer.isValid()){
                LOG_ERROR(LOG_RESOURCE, "Failed to create vertex buffer for: {}", request.path.string());
                return Mesh{};
            }

            // Create index buffer (if present)
            RHIBufferHandle indexBuffer{};
            if(!submesh.indices.empty()){
                RHIBufferCreateDesc indexBufferDesc{
                    .size = submesh.indices.size() * sizeof(uint32_t),
                    .usage = BufIndexBuffer,
                    .stride = 0,
                    .initialData = submesh.indices.data(),
                    .debugName = indexDebugName.c_str()
                };

                indexBuffer = request.device->createBuffer(indexBufferDesc);
                if(!indexBuffer.isValid()){
                    LOG_ERROR(LOG_RESOURCE, "Failed to create index buffer for: {}", request.path.string());
                    // Clean up vertex buffer
                    request.device->destroyBuffer(vertexBuffer);
                    return Mesh{};
                }
            }

            LOG_INFO(LOG_RESOURCE, "Loaded mesh: {} ({} verts, {} indices)",
                request.path.string(),
                submesh.vertices.size(),
                submesh.indices.size());

            return Mesh{
                .vertexBuffer = vertexBuffer,
                .indexBuffer = indexBuffer,
                .vertexCount = static_cast<uint32_t>(submesh.vertices.size()),
                .indexCount = static_cast<uint32_t>(submesh.indices.size()),
                .vertexLayout = {}  // TODO: Define vertex layout
            };
        }
    };
}
