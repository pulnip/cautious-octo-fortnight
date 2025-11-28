#pragma once

#include "generic_handle.hpp"

namespace RenderToy
{
    // Forward declarations
    class RHIDevice;
    class RHICommandList;
    class RHIResource;
    class RHIBuffer;
    class RHITexture;
    class RHIShader;
    class RHIPipelineState;
    class RHISwapchain;
    class RHIFence;

    // Handle types using generic_handle from Core library
    // These provide generational index safety and integrate with slot_map
    using RHIBufferHandle = generic_handle<RHIBuffer>;
    using RHITextureHandle = generic_handle<RHITexture>;
    using RHIShaderHandle = generic_handle<RHIShader>;
    using RHIPipelineStateHandle = generic_handle<RHIPipelineState>;
    using RHISwapchainHandle = generic_handle<RHISwapchain>;
    using RHIFenceHandle = generic_handle<RHIFence>;

    // Hash functors for using handles in unordered containers
    using RHIBufferHandleHash = generic_handleHash<RHIBuffer>;
    using RHITextureHandleHash = generic_handleHash<RHITexture>;
    using RHIShaderHandleHash = generic_handleHash<RHIShader>;
    using RHIPipelineStateHandleHash = generic_handleHash<RHIPipelineState>;
    using RHISwapchainHandleHash = generic_handleHash<RHISwapchain>;
    using RHIFenceHandleHash = generic_handleHash<RHIFence>;

    // Invalid handle constants
    constexpr RHIBufferHandle RHI_INVALID_BUFFER_HANDLE = {};
    constexpr RHITextureHandle RHI_INVALID_TEXTURE_HANDLE = {};
    constexpr RHIShaderHandle RHI_INVALID_SHADER_HANDLE = {};
    constexpr RHIPipelineStateHandle RHI_INVALID_PIPELINE_STATE_HANDLE = {};
    constexpr RHISwapchainHandle RHI_INVALID_SWAPCHAIN_HANDLE = {};
    constexpr RHIFenceHandle RHI_INVALID_FENCE_HANDLE = {};
}
