#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "Log/Log.hpp"
#include "Resource/ResourceTraits.hpp"
#include "Resource/Submesh.hpp"
#include "Content/MeshFormat.hpp"
#include "Importer/MeshImporter.hpp"
#include "RHI/RHIDevice.hpp"

namespace RenderToy
{
    struct SubmeshRequest{
        std::filesystem::path path;
        uint32_t submeshIndex = 0;  // Which submesh in the file
        RHIDevice* device = nullptr;  // Required for GPU upload
    };

    struct SubmeshKey{
        std::string canonicalPath;
        uint32_t submeshIndex;

        auto operator<=>(const SubmeshKey&) const = default;
    };

    struct SubmeshKeyHash{
        inline size_t operator()(const SubmeshKey& k) const noexcept{
            size_t h1 = std::hash<std::string>{}(k.canonicalPath);
            size_t h2 = std::hash<uint32_t>{}(k.submeshIndex);
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct ResourceTraits<Submesh>{
        using Request = SubmeshRequest;
        using Key     = SubmeshKey;
        using KeyHash = SubmeshKeyHash;

        inline static Key makeKey(const Request& request){
            auto canonical = std::filesystem::weakly_canonical(request.path);
            return Key{
                .canonicalPath = canonical.string(),
                .submeshIndex = request.submeshIndex
            };
        }

        inline static Submesh load(const Request& request){
            if(!request.device){
                LOG_ERROR(LOG_RESOURCE, "SubmeshRequest.device is null");
                return Submesh{};
            }

            // Determine file type and load accordingly
            auto ext = request.path.extension().string();
            std::optional<MeshData> meshData;

            if(ext == ".rtmesh"){
                // Load from binary format - read file into memory first
                std::ifstream file(request.path, std::ios::binary | std::ios::ate);
                if(!file.is_open()){
                    LOG_ERROR(LOG_RESOURCE, "Failed to open file: {}", request.path.string());
                    return Submesh{};
                }

                auto fileSize = file.tellg();
                file.seekg(0, std::ios::beg);

                std::vector<uint8_t> buffer(fileSize);
                file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
                file.close();

                meshData = deserializeMesh(buffer);
            }
            else if(ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".pmx"){
                // Load from 3D model file
                meshData = importMesh(request.path.string());
            }
            else {
                LOG_ERROR(LOG_RESOURCE, "Unsupported mesh file format: {}", ext);
                return Submesh{};
            }

            if(!meshData.has_value()){
                LOG_ERROR(LOG_RESOURCE, "Failed to load mesh: {}", request.path.string());
                return Submesh{};
            }

            // Check if requested submesh index is valid
            if(request.submeshIndex >= meshData->submeshes.size()){
                LOG_ERROR(LOG_RESOURCE, "Submesh index {} out of range (mesh has {} submeshes)",
                    request.submeshIndex, meshData->submeshes.size());
                return Submesh{};
            }

            const auto& submesh = meshData->submeshes[request.submeshIndex];

            // Create debug names (persistent strings)
            std::string vertexDebugName = request.path.filename().string() +
                                         "_submesh" + std::to_string(request.submeshIndex) + "_vb";
            std::string indexDebugName = request.path.filename().string() +
                                        "_submesh" + std::to_string(request.submeshIndex) + "_ib";

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
                return Submesh{};
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
                    return Submesh{};
                }
            }

            LOG_INFO(LOG_RESOURCE, "Loaded submesh {}: {} ({} verts, {} indices)",
                request.submeshIndex,
                request.path.string(),
                submesh.vertices.size(),
                submesh.indices.size());

            return Submesh{
                .vertexBuffer = vertexBuffer,
                .indexBuffer = indexBuffer,
                .vertexCount = static_cast<uint32_t>(submesh.vertices.size()),
                .indexCount = static_cast<uint32_t>(submesh.indices.size()),
                .vertexStride = sizeof(Vertex),
                .vertexLayout = {}  // TODO: Define vertex layout
            };
        }
    };
}
