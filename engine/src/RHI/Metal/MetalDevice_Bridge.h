/*
 * MetalDevice C Bridge Header
 *
 * This file provides C-compatible interfaces for Swift implementation
 * of the Metal RHI backend. All functions follow the pattern:
 * ClassName_methodName(objectPtr, ...)
 */

#pragma once

#include "RHI/RHIDesc.h"
#include "MetalTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// MetalDevice C Bridge Functions
// ============================================================================

// Device lifecycle
MetalDevicePtr MetalDevice_create(
    const RHIDeviceCreateDesc* desc);
void MetalDevice_destroy(
    MetalDevicePtr device);

// Device properties
const char* MetalDevice_getDeviceName(
    MetalDevicePtr device);
const char* MetalDevice_getAPIName(
    MetalDevicePtr device);

// Buffer management (using uint64_t handle IDs for Swift compatibility)
MetalBufferID MetalDevice_createBuffer(
    MetalDevicePtr device,
    const RHIBufferCreateDesc* desc);
void MetalDevice_destroyBuffer(
    MetalDevicePtr device,
    MetalBufferID bufferID);
void MetalDevice_uploadBufferData(
    MetalDevicePtr device,
    MetalBufferID bufferID,
    const void* data,
    size_t size,
    size_t offset
);
void MetalDevice_updateBuffer(
    MetalDevicePtr device,
    MetalBufferID bufferID,
    const void* data,
    size_t size,
    size_t offset
);

// Texture management
MetalTextureID MetalDevice_createTexture(
    MetalDevicePtr device,
    const RHITextureCreateDesc* desc
);
void MetalDevice_destroyTexture(
    MetalDevicePtr device,
    MetalTextureID textureID
);
void MetalDevice_uploadTextureData(
    MetalDevicePtr device,
    MetalTextureID textureID,
    const void* data,
    size_t dataSize,
    uint32_t mipLevel,
    uint32_t arraySlice
);

// Shader management
MetalShaderID MetalDevice_createShader(
    MetalDevicePtr device,
    const RHIShaderCreateDesc* desc
);
void MetalDevice_destroyShader(
    MetalDevicePtr device,
    MetalShaderID shaderID
);

// Pipeline state management
MetalPipelineStateID MetalDevice_createGraphicsPipelineState(
    MetalDevicePtr device,
    const RHIGraphicsPipelineStateDesc* desc
);
MetalPipelineStateID MetalDevice_createComputePipelineState(
    MetalDevicePtr device,
    const RHIComputePipelineStateDesc* desc
);
void MetalDevice_destroyPipelineState(
    MetalDevicePtr device,
    MetalPipelineStateID pipelineID
);

// Swapchain management
MetalSwapchainID MetalDevice_createSwapchain(
    MetalDevicePtr device,
    const RHISwapchainCreateDesc* desc
);
void MetalDevice_destroySwapchain(
    MetalDevicePtr device,
    MetalSwapchainID swapchainID
);
bool MetalDevice_resizeSwapchain(
    MetalDevicePtr device,
    MetalSwapchainID swapchainID,
    uint32_t width,
    uint32_t height
);
bool MetalDevice_present(
    MetalDevicePtr device,
    MetalSwapchainID swapchain);
MetalTextureID MetalDevice_getSwapchainBackbuffer(
    MetalDevicePtr device,
    MetalSwapchainID swapchain
);
uint32_t MetalDevice_getSwapchainCurrentIndex(
    MetalDevicePtr device,
    MetalSwapchainID swapchain
);

// Fence management
MetalFenceID MetalDevice_createFence(
    MetalDevicePtr device,
    uint64_t initialValue
);
void MetalDevice_destroyFence(
    MetalDevicePtr device,
    MetalFenceID fenceID
);
void MetalDevice_waitForFence(
    MetalDevicePtr device,
    MetalFenceID fenceID,
    uint64_t value
);
void MetalDevice_signalFence(
    MetalDevicePtr device,
    MetalFenceID fenceID,
    uint64_t value
);
uint64_t MetalDevice_getFenceValue(
    MetalDevicePtr device,
    MetalFenceID fenceID
);
bool MetalDevice_isFenceComplete(
    MetalDevicePtr device,
    MetalFenceID fenceID,
    uint64_t value
);

// Command list management
MetalCommandListPtr MetalDevice_beginCommandList(
    MetalDevicePtr device
);
void MetalDevice_submitCommandList(
    MetalDevicePtr device,
    MetalCommandListPtr cmdList
);
void MetalDevice_submitCommandListWithFence(
    MetalDevicePtr device,
    MetalCommandListPtr cmdList,
    MetalFenceID fenceID,
    uint64_t signalValue
);

// Synchronization
void MetalDevice_waitForIdle(
    MetalDevicePtr device
);

// Metal-specific: Get drawable from swapchain (returns opaque id pointer)
void* MetalDevice_getSwapchainDrawable(
    MetalDevicePtr device,
    MetalSwapchainID swapchainID
);

#ifdef __cplusplus
}
#endif
