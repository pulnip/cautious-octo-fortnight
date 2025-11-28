#pragma once

#include <cstdint>
#include "RHI/RHITypes.hpp"
#include "RHI/RHIDevice.hpp"

namespace RenderToy
{
    // CPU-side mesh resource
    // Holds references to GPU vertex and index buffers
    struct Mesh{
        RHIBufferHandle vertexBuffer;  // GPU vertex buffer
        RHIBufferHandle indexBuffer;   // GPU index buffer
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
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
