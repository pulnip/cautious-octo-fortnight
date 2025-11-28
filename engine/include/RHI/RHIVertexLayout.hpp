#pragma once

#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"

namespace RenderToy
{
    // Simplified vertex layout helpers
    // Full vertex format definitions will be added when implementing actual rendering
    namespace RHIVertexLayoutHelpers
    {
        // Helper to create a vertex element
        inline RHIVertexElement makeElement(
            const char* semanticName,
            uint32_t semanticIndex,
            RHITextureFormat format,
            uint32_t inputSlot,
            uint32_t alignedByteOffset,
            RHIInputClassification classification = RHIInputClassification::PerVertex)
        {
            RHIVertexElement elem{
                .semanticName = nullptr,
                .semanticIndex = semanticIndex,
                .format = format,
                .inputSlot = inputSlot,
                .alignedByteOffset = alignedByteOffset,
                .classification = classification,
                .instanceDataStepRate = 0
            };
            return elem;
        }
    }
}
