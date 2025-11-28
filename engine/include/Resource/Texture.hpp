#pragma once

#include <cstdint>
#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.h"

namespace RenderToy
{
    // CPU-side texture resource
    // Holds reference to GPU texture via RHI handle
    struct Texture{
        RHITextureHandle rhiTexture;  // GPU texture handle
        uint32_t width = 0;
        uint32_t height = 0;
        RHITextureFormat format = RHITextureFormat::Unknown;
        uint32_t mipLevels = 1;

        inline bool isValid() const {
            return rhiTexture.isValid() && width > 0 && height > 0;
        }
    };
}
