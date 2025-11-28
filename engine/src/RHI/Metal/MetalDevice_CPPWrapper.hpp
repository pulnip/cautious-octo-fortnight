#pragma once

#include "RHI/RHIDevice.hpp"
#include "RHI/RHICommandList.hpp"
#include "MetalTypes.h"
#include <memory>
#include <string>

namespace RenderToy
{
    // Forward declaration
    class MetalCommandList;

    // C++ wrapper for Swift MetalDevice implementation
    // This class implements the RHIDevice interface and forwards calls to Swift via C bridge
    class MetalDevice final: public RHIDevice{
        friend class MetalCommandList;

    public:
        MetalDevice();
        ~MetalDevice() override;

        // Initialize the Metal device
        bool initialize(const RHIDeviceCreateDesc& desc);

        // Device properties
        const char* getDeviceName() const override;
        const char* getAPIName() const override;

        // Buffer creation
        RHIBufferHandle createBuffer(
            const RHIBufferCreateDesc& desc) override;
        void destroyBuffer(RHIBufferHandle handle) override;

        // Texture creation
        RHITextureHandle createTexture(
            const RHITextureCreateDesc& desc) override;
        void destroyTexture(RHITextureHandle handle) override;

        // Shader creation
        RHIShaderHandle createShader(
            const RHIShaderCreateDesc& desc) override;
        void destroyShader(RHIShaderHandle handle) override;

        // Pipeline state creation
        RHIPipelineStateHandle createGraphicsPipelineState(
            const RHIGraphicsPipelineStateDesc& desc) override;
        RHIPipelineStateHandle createComputePipelineState(
            const RHIComputePipelineStateDesc& desc) override;
        void destroyPipelineState(RHIPipelineStateHandle handle) override;

        // Swapchain creation
        RHISwapchainHandle createSwapchain(const RHISwapchainCreateDesc& desc) override;
        void destroySwapchain(RHISwapchainHandle handle) override;

        bool resizeSwapchain(
            RHISwapchainHandle swapchain,
            uint32_t width,
            uint32_t height) override;
        bool present(
            RHISwapchainHandle swapchain) override;
        RHITextureHandle getSwapchainBackbuffer(
            RHISwapchainHandle swapchain) override;
        uint32_t getSwapchainCurrentIndex(
            RHISwapchainHandle swapchain) override;

        // Fence creation
        RHIFenceHandle createFence(
            uint64_t initialValue = 0) override;
        void destroyFence(
            RHIFenceHandle handle) override;

        // Command list management
        RHICommandList* beginCommandList() override;
        void submitCommandList(
            RHICommandList* cmdList) override;
        void submitCommandList(
            RHICommandList* cmdList,
            RHIFenceHandle fence,
            uint64_t signalValue) override;

        // Synchronization
        void waitForIdle() override;
        void waitForFence(RHIFenceHandle fence, uint64_t value) override;
        void signalFence(RHIFenceHandle fence, uint64_t value) override;
        uint64_t getFenceValue(RHIFenceHandle fence) override;
        bool isFenceComplete(RHIFenceHandle fence, uint64_t value) override;

        // Resource data upload helpers
        void uploadBufferData(
            RHIBufferHandle buffer,
            const void* data,
            size_t size,
            size_t offset = 0) override;

        void updateBuffer(
            RHIBufferHandle buffer,
            const void* data,
            size_t size,
            size_t offset = 0) override;

        void uploadTextureData(
            RHITextureHandle texture,
            const void* data,
            size_t dataSize,
            uint32_t mipLevel = 0,
            uint32_t arraySlice = 0) override;

        // Resource state transitions (no-op for Metal - automatic)
        void transitionResource(
            RHITextureHandle texture,
            RHIResourceState before,
            RHIResourceState after) override;

        void transitionResource(
            RHIBufferHandle buffer,
            RHIResourceState before,
            RHIResourceState after) override;

        // Metal-specific accessor (for command list)
        inline MetalDevicePtr getSwiftDevice() const{ return swiftDevice; }
        void* getSwapchainDrawable(RHISwapchainHandle handle);

    private:
        MetalDevicePtr swiftDevice; // Opaque pointer to Swift MetalDevice object
        std::string deviceName;
    };
}
