#pragma once

#include <cstdint>
#include "RHI/RHITypes.hpp"
#include "RHI/RHIDevice.hpp"

namespace RenderToy
{
    // CPU-side submesh resource
    // Represents a single drawable unit with vertex/index buffers
    // A complete mesh (e.g., car.gltf) may contain multiple submeshes
    struct Submesh{
        RHIBufferHandle vertexBuffer;  // GPU vertex buffer
        RHIBufferHandle indexBuffer;   // GPU index buffer
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        uint32_t vertexStride = 0;     // Added: stride for vertex data
        RHIVertexLayout vertexLayout = {
            .elements = nullptr,
            .elementCount = 0
        };

        inline bool isValid() const {
            return vertexBuffer.isValid() && vertexCount > 0;
        }

        inline bool hasIndices() const {
            return indexBuffer.isValid() && indexCount > 0;
        }
    };
}
