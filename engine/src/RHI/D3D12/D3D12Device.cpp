#include "D3D12Device.hpp"
#include "D3D12Swapchain.hpp"
#include "D3D12Fence.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12Buffer.hpp"
#include "D3D12Texture.hpp"
#include "D3D12Shader.hpp"
#include "D3D12PipelineState.hpp"
#include "RHI/Platform.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace RenderToy
{
    D3D12Device::D3D12Device()
        : device(nullptr)
        , commandQueue(nullptr)
        , dxgiFactory(nullptr)
        , adapter(nullptr)
    {
    }

    D3D12Device::~D3D12Device()
    {
        // Wait for any pending work to complete
        if (commandQueue) {
            waitForIdle();
        }

        // Destroy all swapchains
        for (auto& [id, swapchain] : swapchains) {
            if (swapchain) {
                swapchain->cleanup();
                delete swapchain;
            }
        }
        swapchains.clear();

        // Destroy all fences
        for (auto& [id, fence] : fences) {
            if (fence) {
                fence->cleanup();
                delete fence;
            }
        }
        fences.clear();

        // Destroy all buffers
        for (auto& [id, buffer] : buffers) {
            if (buffer) {
                buffer->cleanup();
                delete buffer;
            }
        }
        buffers.clear();

        // Destroy all textures
        for (auto& [id, texture] : textures) {
            if (texture) {
                texture->cleanup();
                delete texture;
            }
        }
        textures.clear();

        // Destroy all shaders
        for (auto& [id, shader] : shaders) {
            if (shader) {
                shader->cleanup();
                delete shader;
            }
        }
        shaders.clear();

        // Destroy all pipeline states
        for (auto& [id, pso] : pipelineStates) {
            if (pso) {
                pso->cleanup();
                delete pso;
            }
        }
        pipelineStates.clear();

        // Release D3D12 objects
        if (commandQueue) {
            commandQueue->Release();
            commandQueue = nullptr;
        }
        if (device) {
            device->Release();
            device = nullptr;
        }
        if (adapter) {
            adapter->Release();
            adapter = nullptr;
        }
        if (dxgiFactory) {
            dxgiFactory->Release();
            dxgiFactory = nullptr;
        }

        LOG_INFO(LOG_RHI, "D3D12 device destroyed");
    }

    bool D3D12Device::initialize(const RHIDeviceCreateDesc& desc)
    {
        LOG_INFO(LOG_RHI, "Initializing D3D12 device...");

        // Enable debug layer if requested
        if (desc.enableDebugLayer || desc.enableValidation) {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
                LOG_INFO(LOG_RHI, "D3D12 debug layer enabled");
            }
        }

        // Create DXGI factory
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)))) {
            LOG_ERROR(LOG_RHI, "Failed to create DXGI factory");
            return false;
        }

        // Find suitable adapter
        ComPtr<IDXGIAdapter1> tempAdapter;
        for (UINT adapterIndex = 0;
             dxgiFactory->EnumAdapters1(adapterIndex, &tempAdapter) != DXGI_ERROR_NOT_FOUND;
             ++adapterIndex) {

            DXGI_ADAPTER_DESC1 desc;
            tempAdapter->GetDesc1(&desc);

            // Skip software adapter
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }

            // Try to create device with this adapter
            if (SUCCEEDED(D3D12CreateDevice(tempAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                           IID_PPV_ARGS(&device)))) {
                adapter = tempAdapter.Detach();
                break;
            }
        }

        if (!device) {
            LOG_ERROR(LOG_RHI, "Failed to create D3D12 device - no suitable GPU found");
            return false;
        }

        // Get device name
        DXGI_ADAPTER_DESC1 adapterDesc;
        adapter->GetDesc1(&adapterDesc);
        char deviceNameBuffer[256];
        wcstombs(deviceNameBuffer, adapterDesc.Description, sizeof(deviceNameBuffer));
        deviceName = deviceNameBuffer;
        LOG_INFO(LOG_RHI, "D3D12 device: {}", deviceName);

        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) {
            LOG_ERROR(LOG_RHI, "Failed to create D3D12 command queue");
            return false;
        }

        LOG_INFO(LOG_RHI, "D3D12 device initialized successfully");
        return true;
    }

    const char* D3D12Device::getDeviceName() const
    {
        return deviceName.c_str();
    }

    const char* D3D12Device::getAPIName() const
    {
        return "Direct3D 12";
    }

    // Buffer creation
    RHIBufferHandle D3D12Device::createBuffer(const RHIBufferCreateDesc& desc)
    {
        auto* buffer = new D3D12Buffer();
        if (!buffer->initialize(device, desc)) {
            delete buffer;
            return RHIBufferHandle{};
        }

        uint64_t id = nextBufferId++;
        buffers[id] = buffer;

        return makeBufferHandle(id);
    }

    void D3D12Device::destroyBuffer(RHIBufferHandle handle)
    {
        auto* buffer = getBuffer(handle);
        if (buffer) {
            buffer->cleanup();
            delete buffer;
            buffers.erase(handle.index);
        }
    }

    // Texture creation
    RHITextureHandle D3D12Device::createTexture(const RHITextureCreateDesc& desc)
    {
        auto* texture = new D3D12Texture();
        if (!texture->initialize(device, desc)) {
            delete texture;
            return RHITextureHandle{};
        }

        uint64_t id = nextTextureId++;
        textures[id] = texture;

        return makeTextureHandle(id);
    }

    void D3D12Device::destroyTexture(RHITextureHandle handle)
    {
        auto* texture = getTexture(handle);
        if (texture) {
            texture->cleanup();
            delete texture;
            textures.erase(handle.index);
        }
    }

    // Shader creation
    RHIShaderHandle D3D12Device::createShader(const RHIShaderCreateDesc& desc)
    {
        auto* shader = new D3D12Shader();
        if (!shader->initialize(device, desc)) {
            delete shader;
            return RHIShaderHandle{};
        }

        uint64_t id = nextShaderId++;
        shaders[id] = shader;

        return makeShaderHandle(id);
    }

    void D3D12Device::destroyShader(RHIShaderHandle handle)
    {
        auto* shader = getShader(handle);
        if (shader) {
            shader->cleanup();
            delete shader;
            shaders.erase(handle.index);
        }
    }

    // Pipeline state creation
    RHIPipelineStateHandle D3D12Device::createGraphicsPipelineState(
        const RHIGraphicsPipelineStateDesc& desc)
    {
        auto* pso = new D3D12PipelineState();

        // Get shaders from handles
        auto* vertexShader = getShader(desc.vertexShader);
        auto* fragmentShader = getShader(desc.pixelShader);

        if (!pso->initializeGraphics(device, desc, vertexShader, fragmentShader)) {
            delete pso;
            return RHIPipelineStateHandle{};
        }

        uint64_t id = nextPipelineStateId++;
        pipelineStates[id] = pso;

        return makePipelineStateHandle(id);
    }

    RHIPipelineStateHandle D3D12Device::createComputePipelineState(
        const RHIComputePipelineStateDesc& desc)
    {
        auto* pso = new D3D12PipelineState();

        // Get compute shader from handle
        auto* computeShader = getShader(desc.computeShader);

        if (!pso->initializeCompute(device, desc, computeShader)) {
            delete pso;
            return RHIPipelineStateHandle{};
        }

        uint64_t id = nextPipelineStateId++;
        pipelineStates[id] = pso;

        return makePipelineStateHandle(id);
    }

    void D3D12Device::destroyPipelineState(RHIPipelineStateHandle handle)
    {
        auto* pso = getPipelineState(handle);
        if (pso) {
            pso->cleanup();
            delete pso;
            pipelineStates.erase(handle.index);
        }
    }

    // Swapchain implementation
    RHISwapchainHandle D3D12Device::createSwapchain(const RHISwapchainCreateDesc& desc)
    {
        auto* swapchain = new D3D12Swapchain();
        if (!swapchain->initialize(device, desc.windowHandle, desc.width, desc.height,
                                   desc.bufferCount, desc.vsync)) {
            delete swapchain;
            return RHISwapchainHandle{};
        }

        uint64_t id = nextSwapchainId++;
        swapchains[id] = swapchain;

        return makeSwapchainHandle(id);
    }

    void D3D12Device::destroySwapchain(RHISwapchainHandle handle)
    {
        auto* swapchain = getSwapchain(handle);
        if (swapchain) {
            swapchain->cleanup();
            delete swapchain;
            swapchains.erase(handle.index);
        }
    }

    bool D3D12Device::resizeSwapchain(RHISwapchainHandle handle, uint32_t width, uint32_t height)
    {
        auto* swapchain = getSwapchain(handle);
        if (swapchain) {
            swapchain->resize(width, height);
            return true;
        }
        return false;
    }

    bool D3D12Device::present(RHISwapchainHandle handle)
    {
        auto* swapchain = getSwapchain(handle);
        if (!swapchain || !swapchain->swapchain) {
            return false;
        }

        // Present the frame
        UINT syncInterval = swapchain->vsyncEnabled ? 1 : 0;
        HRESULT hr = swapchain->swapchain->Present(syncInterval, 0);

        // Acquire next backbuffer for the next frame
        swapchain->acquireNextImage();

        return SUCCEEDED(hr);
    }

    RHITextureHandle D3D12Device::getSwapchainBackbuffer(RHISwapchainHandle handle)
    {
        // TODO: Return actual texture handle for the current backbuffer
        return RHITextureHandle{};
    }

    uint32_t D3D12Device::getSwapchainCurrentIndex(RHISwapchainHandle handle)
    {
        auto* swapchain = getSwapchain(handle);
        return swapchain ? swapchain->currentFrameIndex : 0;
    }

    // Helper methods
    RHISwapchainHandle D3D12Device::makeSwapchainHandle(uint64_t id)
    {
        RHISwapchainHandle handle{};
        handle.index = id;
        handle.generation = 1;
        return handle;
    }

    D3D12Swapchain* D3D12Device::getSwapchain(RHISwapchainHandle handle)
    {
        auto it = swapchains.find(handle.index);
        return (it != swapchains.end()) ? it->second : nullptr;
    }

    RHIFenceHandle D3D12Device::makeFenceHandle(uint64_t id)
    {
        RHIFenceHandle handle{};
        handle.index = id;
        handle.generation = 1;
        return handle;
    }

    D3D12Fence* D3D12Device::getFence(RHIFenceHandle handle)
    {
        auto it = fences.find(handle.index);
        return (it != fences.end()) ? it->second : nullptr;
    }

    RHIBufferHandle D3D12Device::makeBufferHandle(uint64_t id)
    {
        RHIBufferHandle handle{};
        handle.index = id;
        handle.generation = 1;
        return handle;
    }

    D3D12Buffer* D3D12Device::getBuffer(RHIBufferHandle handle)
    {
        auto it = buffers.find(handle.index);
        return (it != buffers.end()) ? it->second : nullptr;
    }

    RHITextureHandle D3D12Device::makeTextureHandle(uint64_t id)
    {
        RHITextureHandle handle{};
        handle.index = id;
        handle.generation = 1;
        return handle;
    }

    D3D12Texture* D3D12Device::getTexture(RHITextureHandle handle)
    {
        auto it = textures.find(handle.index);
        return (it != textures.end()) ? it->second : nullptr;
    }

    RHIShaderHandle D3D12Device::makeShaderHandle(uint64_t id)
    {
        RHIShaderHandle handle{};
        handle.index = id;
        handle.generation = 1;
        return handle;
    }

    D3D12Shader* D3D12Device::getShader(RHIShaderHandle handle)
    {
        auto it = shaders.find(handle.index);
        return (it != shaders.end()) ? it->second : nullptr;
    }

    RHIPipelineStateHandle D3D12Device::makePipelineStateHandle(uint64_t id)
    {
        RHIPipelineStateHandle handle{};
        handle.index = id;
        handle.generation = 1;
        return handle;
    }

    D3D12PipelineState* D3D12Device::getPipelineState(RHIPipelineStateHandle handle)
    {
        auto it = pipelineStates.find(handle.index);
        return (it != pipelineStates.end()) ? it->second : nullptr;
    }

    // Fence implementation
    RHIFenceHandle D3D12Device::createFence(uint64_t initialValue)
    {
        auto* fence = new D3D12Fence();
        if (!fence->initialize(device, initialValue)) {
            delete fence;
            return RHIFenceHandle{};
        }

        uint64_t id = nextFenceId++;
        fences[id] = fence;

        return makeFenceHandle(id);
    }

    void D3D12Device::destroyFence(RHIFenceHandle handle)
    {
        auto* fence = getFence(handle);
        if (fence) {
            fence->cleanup();
            delete fence;
            fences.erase(handle.index);
        }
    }

    // Command list implementation
    RHICommandList* D3D12Device::beginCommandList()
    {
        // TODO: Implement command list pooling
        // For now, create a temporary command allocator
        ID3D12CommandAllocator* allocator = nullptr;
        if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                  IID_PPV_ARGS(&allocator)))) {
            LOG_ERROR(LOG_RHI, "Failed to create command allocator");
            return nullptr;
        }

        auto* cmdList = new D3D12CommandList(allocator, this);
        cmdList->begin();
        return cmdList;
    }

    void D3D12Device::submitCommandList(RHICommandList* cmdList)
    {
        if (!cmdList) {
            LOG_ERROR(LOG_RHI, "Cannot submit null command list");
            return;
        }

        auto* d3d12CmdList = static_cast<D3D12CommandList*>(cmdList);
        d3d12CmdList->close();

        // Submit to command queue
        ID3D12CommandList* cmdLists[] = { d3d12CmdList->getD3D12CommandList() };
        commandQueue->ExecuteCommandLists(1, cmdLists);

        // Clean up
        delete d3d12CmdList;
    }

    void D3D12Device::submitCommandList(RHICommandList* cmdList,
                                       RHIFenceHandle fence,
                                       uint64_t signalValue)
    {
        if (!cmdList) {
            LOG_ERROR(LOG_RHI, "Cannot submit null command list");
            return;
        }

        auto* d3d12CmdList = static_cast<D3D12CommandList*>(cmdList);
        auto* d3d12Fence = getFence(fence);

        d3d12CmdList->close();

        // Submit to command queue
        ID3D12CommandList* cmdLists[] = { d3d12CmdList->getD3D12CommandList() };
        commandQueue->ExecuteCommandLists(1, cmdLists);

        // Signal fence after execution
        if (d3d12Fence && d3d12Fence->fence) {
            commandQueue->Signal(d3d12Fence->fence, signalValue);
        }

        // Clean up
        delete d3d12CmdList;
    }

    // Synchronization
    void D3D12Device::waitForIdle()
    {
        // Create a temporary fence and wait for it
        auto fenceHandle = createFence(0);
        signalFence(fenceHandle, 1);
        waitForFence(fenceHandle, 1);
        destroyFence(fenceHandle);
    }

    void D3D12Device::waitForFence(RHIFenceHandle handle, uint64_t value)
    {
        auto* fence = getFence(handle);
        if (fence) {
            fence->wait(value);
        }
    }

    void D3D12Device::signalFence(RHIFenceHandle handle, uint64_t value)
    {
        auto* fence = getFence(handle);
        if (fence && fence->fence) {
            commandQueue->Signal(fence->fence, value);
        }
    }

    uint64_t D3D12Device::getFenceValue(RHIFenceHandle handle)
    {
        auto* fence = getFence(handle);
        return fence ? fence->getValue() : 0;
    }

    bool D3D12Device::isFenceComplete(RHIFenceHandle handle, uint64_t value)
    {
        auto* fence = getFence(handle);
        return fence ? fence->isComplete(value) : true;
    }

    // Resource upload
    void D3D12Device::uploadBufferData(RHIBufferHandle bufferHandle,
                                      const void* data,
                                      size_t size,
                                      size_t offset)
    {
        auto* buffer = getBuffer(bufferHandle);
        if (buffer) {
            buffer->upload(data, size, offset);
        }
    }

    void D3D12Device::updateBuffer(RHIBufferHandle bufferHandle,
                                  const void* data,
                                  size_t size,
                                  size_t offset)
    {
        auto* buffer = getBuffer(bufferHandle);
        if (buffer) {
            buffer->upload(data, size, offset);
        }
    }

    void D3D12Device::uploadTextureData(RHITextureHandle textureHandle,
                                       const void* data,
                                       size_t dataSize,
                                       uint32_t mipLevel,
                                       uint32_t arraySlice)
    {
        auto* texture = getTexture(textureHandle);
        if (texture) {
            texture->upload(data, dataSize, mipLevel, arraySlice);
        }
    }

    // Resource transitions (important for D3D12!)
    void D3D12Device::transitionResource(RHITextureHandle texture,
                                        RHIResourceState before,
                                        RHIResourceState after)
    {
        // TODO: Implement resource transitions
        // For now, just update the tracked state
        auto* tex = getTexture(texture);
        if (tex) {
            tex->currentState = after;
        }
    }

    void D3D12Device::transitionResource(RHIBufferHandle buffer,
                                        RHIResourceState before,
                                        RHIResourceState after)
    {
        // TODO: Implement resource transitions
        // For now, just update the tracked state
        auto* buf = getBuffer(buffer);
        if (buf) {
            buf->currentState = after;
        }
    }

    // Factory function
    std::unique_ptr<RHIDevice> createD3D12Device(const RHIDeviceCreateDesc& desc)
    {
        auto device = std::make_unique<D3D12Device>();
        if (!device->initialize(desc)) {
            LOG_ERROR(LOG_RHI, "Failed to initialize D3D12 device");
            return nullptr;
        }

        return std::unique_ptr<RHIDevice>(std::move(device));
    }
}

#endif // RHI_D3D12
