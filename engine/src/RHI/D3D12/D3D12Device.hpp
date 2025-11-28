#pragma once

#include "RHI/RHIDevice.hpp"
#include "RHI/RHICommandList.hpp"
#include <memory>
#include <string>
#include <unordered_map>

// Forward declarations for D3D12
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct IDXGIFactory4;
struct IDXGIAdapter1;

namespace RenderToy
{
    // Forward declaration
    class D3D12CommandList;

    // D3D12-specific device implementation
    class D3D12Device final : public RHIDevice
    {
        friend class D3D12CommandList;

    public:
        D3D12Device();
        ~D3D12Device() override;

        // Initialize the D3D12 device
        bool initialize(const RHIDeviceCreateDesc& desc);

        // Device properties
        const char* getDeviceName() const override;
        const char* getAPIName() const override;

        // Buffer creation
        RHIBufferHandle createBuffer(const RHIBufferCreateDesc& desc) override;
        void destroyBuffer(RHIBufferHandle handle) override;

        // Texture creation
        RHITextureHandle createTexture(const RHITextureCreateDesc& desc) override;
        void destroyTexture(RHITextureHandle handle) override;

        // Shader creation
        RHIShaderHandle createShader(const RHIShaderCreateDesc& desc) override;
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
        bool resizeSwapchain(RHISwapchainHandle swapchain, uint32_t width, uint32_t height) override;
        bool present(RHISwapchainHandle swapchain) override;
        RHITextureHandle getSwapchainBackbuffer(RHISwapchainHandle swapchain) override;
        uint32_t getSwapchainCurrentIndex(RHISwapchainHandle swapchain) override;

        // Fence creation
        RHIFenceHandle createFence(uint64_t initialValue = 0) override;
        void destroyFence(RHIFenceHandle handle) override;

        // Command list management
        RHICommandList* beginCommandList() override;
        void submitCommandList(RHICommandList* cmdList) override;
        void submitCommandList(RHICommandList* cmdList,
                              RHIFenceHandle fence,
                              uint64_t signalValue) override;

        // Synchronization
        void waitForIdle() override;
        void waitForFence(RHIFenceHandle fence, uint64_t value) override;
        void signalFence(RHIFenceHandle fence, uint64_t value) override;
        uint64_t getFenceValue(RHIFenceHandle fence) override;
        bool isFenceComplete(RHIFenceHandle fence, uint64_t value) override;

        // Resource data upload helpers
        void uploadBufferData(RHIBufferHandle buffer,
                             const void* data,
                             size_t size,
                             size_t offset = 0) override;

        void updateBuffer(RHIBufferHandle buffer,
                         const void* data,
                         size_t size,
                         size_t offset = 0) override;

        void uploadTextureData(RHITextureHandle texture,
                              const void* data,
                              size_t dataSize,
                              uint32_t mipLevel = 0,
                              uint32_t arraySlice = 0) override;

        // Resource state transitions (critical for D3D12)
        void transitionResource(RHITextureHandle texture,
                               RHIResourceState before,
                               RHIResourceState after) override;

        void transitionResource(RHIBufferHandle buffer,
                               RHIResourceState before,
                               RHIResourceState after) override;

        // D3D12-specific accessors
        ID3D12Device* getD3D12Device() const { return device; }
        ID3D12CommandQueue* getD3D12CommandQueue() const { return commandQueue; }

    private:
        ID3D12Device* device;
        ID3D12CommandQueue* commandQueue;
        IDXGIFactory4* dxgiFactory;
        IDXGIAdapter1* adapter;

        std::string deviceName;

        // Resource storage
        // Using simple maps for now - will optimize with slot_map later
        std::unordered_map<uint64_t, struct D3D12Swapchain*> swapchains;
        std::unordered_map<uint64_t, struct D3D12Fence*> fences;
        std::unordered_map<uint64_t, struct D3D12Buffer*> buffers;
        std::unordered_map<uint64_t, struct D3D12Texture*> textures;
        std::unordered_map<uint64_t, struct D3D12Shader*> shaders;
        std::unordered_map<uint64_t, struct D3D12PipelineState*> pipelineStates;
        uint64_t nextSwapchainId = 1;
        uint64_t nextFenceId = 1;
        uint64_t nextBufferId = 1;
        uint64_t nextTextureId = 1;
        uint64_t nextShaderId = 1;
        uint64_t nextPipelineStateId = 1;

        // Helper methods
        RHISwapchainHandle makeSwapchainHandle(uint64_t id);
        D3D12Swapchain* getSwapchain(RHISwapchainHandle handle);

        RHIFenceHandle makeFenceHandle(uint64_t id);
        D3D12Fence* getFence(RHIFenceHandle handle);

        RHIBufferHandle makeBufferHandle(uint64_t id);
        D3D12Buffer* getBuffer(RHIBufferHandle handle);

        RHITextureHandle makeTextureHandle(uint64_t id);
        D3D12Texture* getTexture(RHITextureHandle handle);

        RHIShaderHandle makeShaderHandle(uint64_t id);
        D3D12Shader* getShader(RHIShaderHandle handle);

        RHIPipelineStateHandle makePipelineStateHandle(uint64_t id);
        D3D12PipelineState* getPipelineState(RHIPipelineStateHandle handle);
    };
}
