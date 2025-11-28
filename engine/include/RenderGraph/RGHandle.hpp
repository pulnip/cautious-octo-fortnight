#pragma once

#include <cstdint>
#include "generic_handle.hpp"

namespace RenderToy {
    // Forward declarations
    struct RGTexture;
    struct RGBuffer;

    /// @brief Handle to a RenderGraph texture resource
    using RGTextureHandle = generic_handle<RGTexture>;

    /// @brief Handle to a RenderGraph buffer resource
    using RGBufferHandle = generic_handle<RGBuffer>;

    /// @brief Invalid handle constants
    inline constexpr RGTextureHandle RG_INVALID_TEXTURE = {};
    inline constexpr RGBufferHandle RG_INVALID_BUFFER = {};
} // namespace RenderToy
