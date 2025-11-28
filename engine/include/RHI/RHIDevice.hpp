#pragma once

#include <memory>
#include <cstdint>
#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.h"
#include "RHI/RHIDesc.h"
#include "Platform/Window.hpp"

namespace RenderToy
{
    // Main RHI device interface
    // This is the central hub for creating resources and submitting work to GPU
    class RHIDevice
    {
    public:
        virtual ~RHIDevice() = default;

        // Factory method to create platform-specific device
        static std::unique_ptr<RHIDevice> create(const RHIDeviceCreateDesc& desc);

        // Device properties
        virtual const char* getDeviceName() const = 0;
        virtual const char* getAPIName() const = 0;

        // Buffer creation
        virtual RHIBufferHandle createBuffer(const RHIBufferCreateDesc& desc) = 0;
        virtual void destroyBuffer(RHIBufferHandle handle) = 0;

        // Texture creation
        virtual RHITextureHandle createTexture(const RHITextureCreateDesc& desc) = 0;
        virtual void destroyTexture(RHITextureHandle handle) = 0;

        // Shader creation
        virtual RHIShaderHandle createShader(const RHIShaderCreateDesc& desc) = 0;
        virtual void destroyShader(RHIShaderHandle handle) = 0;

        // Pipeline state creation
        virtual RHIPipelineStateHandle createGraphicsPipelineState(
            const RHIGraphicsPipelineStateDesc& desc) = 0;
        virtual RHIPipelineStateHandle createComputePipelineState(
            const RHIComputePipelineStateDesc& desc) = 0;
        virtual void destroyPipelineState(RHIPipelineStateHandle handle) = 0;

        // Swapchain creation
        virtual RHISwapchainHandle createSwapchain(const RHISwapchainCreateDesc& desc) = 0;
        virtual void destroySwapchain(RHISwapchainHandle handle) = 0;

        // Fence creation
        virtual RHIFenceHandle createFence(uint64_t initialValue = 0) = 0;
        virtual void destroyFence(RHIFenceHandle handle) = 0;

        // Command list management
        virtual RHICommandList* beginCommandList() = 0;
        virtual void submitCommandList(RHICommandList* cmdList) = 0;
        virtual void submitCommandList(RHICommandList* cmdList,
                                      RHIFenceHandle fence,
                                      uint64_t signalValue) = 0;

        // Synchronization
        virtual void waitForIdle() = 0;
        virtual void waitForFence(RHIFenceHandle fence, uint64_t value) = 0;
        virtual void signalFence(RHIFenceHandle fence, uint64_t value) = 0;
        virtual uint64_t getFenceValue(RHIFenceHandle fence) = 0;
        virtual bool isFenceComplete(RHIFenceHandle fence, uint64_t value) = 0;

        // Swapchain operations
        virtual bool resizeSwapchain(RHISwapchainHandle swapchain, uint32_t width, uint32_t height) = 0;
        virtual bool present(RHISwapchainHandle swapchain) = 0;
        virtual RHITextureHandle getSwapchainBackbuffer(RHISwapchainHandle swapchain) = 0;
        virtual uint32_t getSwapchainCurrentIndex(RHISwapchainHandle swapchain) = 0;

        // Resource data upload helpers
        virtual void uploadBufferData(RHIBufferHandle buffer,
                                     const void* data,
                                     size_t size,
                                     size_t offset = 0) = 0;

        virtual void updateBuffer(RHIBufferHandle buffer,
                                 const void* data,
                                 size_t size,
                                 size_t offset = 0) = 0;

        virtual void uploadTextureData(RHITextureHandle texture,
                                      const void* data,
                                      size_t dataSize,
                                      uint32_t mipLevel = 0,
                                      uint32_t arraySlice = 0) = 0;

        // Resource state transitions (primarily for D3D12)
        virtual void transitionResource(RHITextureHandle texture,
                                       RHIResourceState before,
                                       RHIResourceState after) = 0;

        virtual void transitionResource(RHIBufferHandle buffer,
                                       RHIResourceState before,
                                       RHIResourceState after) = 0;
    };
}
