#include "MetalDevice_CPPWrapper.hpp"
#include "MetalCommandList_CPPWrapper.hpp"
#include "MetalDevice_Bridge.h"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_METAL

namespace RenderToy
{
    // Helper functions to convert between generic_handle and uint64_t
    template<typename T>
    static uint64_t handleToID(generic_handle<T> handle) {
        return handle.index;
    }

    template<typename T>
    static generic_handle<T> idToHandle(uint64_t id) {
        generic_handle<T> handle;
        handle.index = id;
        handle.generation = 0;  // Generation not used in Swift bridge
        return handle;
    }
    MetalDevice::MetalDevice()
        : swiftDevice(nullptr)
    {
    }

    MetalDevice::~MetalDevice(){
        if(swiftDevice) {
            MetalDevice_destroy(swiftDevice);
            swiftDevice = nullptr;
        }
        LOG_INFO(LOG_METAL, "Metal device destroyed");
    }

    bool MetalDevice::initialize(const RHIDeviceCreateDesc& desc)
    {
        LOG_INFO(LOG_METAL, "Initializing Metal device...");

        // Create Swift MetalDevice via C bridge
        swiftDevice = MetalDevice_create(&desc);
        if(!swiftDevice){
            LOG_ERROR(LOG_METAL, "Failed to create Metal device");
            return false;
        }

        // Cache device name
        auto name = MetalDevice_getDeviceName(swiftDevice);
        if(name){
            deviceName = name;
        }

        LOG_INFO(LOG_METAL, "Metal device initialized successfully: {}", deviceName);
        return true;
    }

    const char* MetalDevice::getDeviceName() const{
        return deviceName.c_str();
    }

    const char* MetalDevice::getAPIName() const{
        return "Metal";
    }

    // Buffer creation
    RHIBufferHandle MetalDevice::createBuffer(
        const RHIBufferCreateDesc& desc)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createBuffer(
            swiftDevice,
            &desc
        );
        return idToHandle<RHIBuffer>(id);
    }

    void MetalDevice::destroyBuffer(
        RHIBufferHandle handle)
    {
        if(!swiftDevice)
            return;
        MetalDevice_destroyBuffer(
            swiftDevice,
            handleToID(handle)
        );
    }

    // Texture creation
    RHITextureHandle MetalDevice::createTexture(
        const RHITextureCreateDesc& desc)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createTexture(
            swiftDevice,
            &desc
        );
        return idToHandle<RHITexture>(id);
    }

    void MetalDevice::destroyTexture(
        RHITextureHandle handle)
    {
        if(!swiftDevice)
            return;
        MetalDevice_destroyTexture(
            swiftDevice,
            handleToID(handle)
        );
    }

    // Shader creation
    RHIShaderHandle MetalDevice::createShader(
        const RHIShaderCreateDesc& desc)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createShader(
            swiftDevice,
            &desc
        );
        return idToHandle<RHIShader>(id);
    }

    void MetalDevice::destroyShader(
        RHIShaderHandle handle)
    {
        if(!swiftDevice)
            return;
        MetalDevice_destroyShader(
            swiftDevice,
            handleToID(handle)
        );
    }

    // Pipeline state creation
    RHIPipelineStateHandle MetalDevice::createGraphicsPipelineState(
        const RHIGraphicsPipelineStateDesc& desc)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createGraphicsPipelineState(
            swiftDevice,
            &desc
        );
        return idToHandle<RHIPipelineState>(id);
    }

    RHIPipelineStateHandle MetalDevice::createComputePipelineState(
        const RHIComputePipelineStateDesc& desc)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createComputePipelineState(
            swiftDevice,
            &desc
        );
        return idToHandle<RHIPipelineState>(id);
    }

    void MetalDevice::destroyPipelineState(
        RHIPipelineStateHandle handle)
    {
        if(!swiftDevice)
            return;
        MetalDevice_destroyPipelineState(
            swiftDevice,
            handleToID(handle)
        );
    }

    // Swapchain creation
    RHISwapchainHandle MetalDevice::createSwapchain(
        const RHISwapchainCreateDesc& desc)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createSwapchain(
            swiftDevice,
            &desc
        );
        return idToHandle<RHISwapchain>(id);
    }

    void MetalDevice::destroySwapchain(
        RHISwapchainHandle handle)
    {
        if(!swiftDevice)
            return;
        MetalDevice_destroySwapchain(
            swiftDevice,
            handleToID(handle)
        );
    }

    bool MetalDevice::resizeSwapchain(
        RHISwapchainHandle swapchain,
        uint32_t width,
        uint32_t height)
    {
        if(!swiftDevice)
            return false;
        return MetalDevice_resizeSwapchain(
            swiftDevice,
            handleToID(swapchain),
            width, 
            height
        );
    }

    bool MetalDevice::present(
        RHISwapchainHandle swapchain)
    {
        if(!swiftDevice)
            return false;
        return MetalDevice_present(
            swiftDevice,
            handleToID(swapchain)
        );
    }

    RHITextureHandle MetalDevice::getSwapchainBackbuffer(
        RHISwapchainHandle swapchain)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_getSwapchainBackbuffer(
            swiftDevice,
            handleToID(swapchain)
        );
        return idToHandle<RHITexture>(id);
    }

    uint32_t MetalDevice::getSwapchainCurrentIndex(
        RHISwapchainHandle swapchain)
    {
        if(!swiftDevice)
            return 0;
        return MetalDevice_getSwapchainCurrentIndex(
            swiftDevice,
            handleToID(swapchain)
        );
    }

    // Fence creation
    RHIFenceHandle MetalDevice::createFence(
        uint64_t initialValue)
    {
        if(!swiftDevice)
            return {};
        auto id = MetalDevice_createFence(
            swiftDevice,
            initialValue
        );
        return idToHandle<RHIFence>(id);
    }

    void MetalDevice::destroyFence(RHIFenceHandle handle)
    {
        if(!swiftDevice)
            return;
        MetalDevice_destroyFence(
            swiftDevice,
            handleToID(handle)
        );
    }

    // Command list management
    RHICommandList* MetalDevice::beginCommandList()
    {
        if(!swiftDevice)
            return nullptr;

        MetalCommandListPtr cmdListHandle = MetalDevice_beginCommandList(swiftDevice);
        if(!cmdListHandle){
            LOG_ERROR(LOG_METAL, "Failed to create command list");
            return nullptr;
        }

        // Create C++ wrapper for command list
        return new MetalCommandList(cmdListHandle, this);
    }

    void MetalDevice::submitCommandList(RHICommandList* cmdList)
    {
        if(!swiftDevice || !cmdList)
            return;

        // Cast to Metal command list wrapper
        auto metalCmdList = static_cast<MetalCommandList*>(cmdList);
        MetalDevice_submitCommandList(
            swiftDevice,
            metalCmdList->getSwiftCommandList()
        );
    }

    void MetalDevice::submitCommandList(
        RHICommandList* cmdList,
        RHIFenceHandle fence,
        uint64_t signalValue)
    {
        if(!swiftDevice || !cmdList)
            return;

        auto metalCmdList = static_cast<MetalCommandList*>(cmdList);
        MetalDevice_submitCommandListWithFence(
            swiftDevice,
            metalCmdList->getSwiftCommandList(),
            handleToID(fence),
            signalValue
        );
    }

    // Synchronization
    void MetalDevice::waitForIdle()
    {
        if(!swiftDevice)
            return;
        MetalDevice_waitForIdle(swiftDevice);
    }

    void MetalDevice::waitForFence(
        RHIFenceHandle fence,
        uint64_t value)
    {
        if(!swiftDevice)
            return;
        MetalDevice_waitForFence(
            swiftDevice,
            handleToID(fence),
            value
        );
    }

    void MetalDevice::signalFence(
        RHIFenceHandle fence,
        uint64_t value)
    {
        if(!swiftDevice)
            return;
        MetalDevice_signalFence(
            swiftDevice,
            handleToID(fence),
            value
        );
    }

    uint64_t MetalDevice::getFenceValue(
        RHIFenceHandle fence)
    {
        if(!swiftDevice)
            return 0;
        return MetalDevice_getFenceValue(
            swiftDevice,
            handleToID(fence)
        );
    }

    bool MetalDevice::isFenceComplete(
        RHIFenceHandle fence,
        uint64_t value)
    {
        if(!swiftDevice)
            return false;
        return MetalDevice_isFenceComplete(
            swiftDevice,
            handleToID(fence),
            value
        );
    }

    // Resource data upload helpers
    void MetalDevice::uploadBufferData(
        RHIBufferHandle buffer,
        const void* data,
        size_t size,
        size_t offset)
    {
        if(!swiftDevice)
            return;
        MetalDevice_uploadBufferData(
            swiftDevice,
            handleToID(buffer),
            data,
            size,
            offset
        );
    }

    void MetalDevice::updateBuffer(
        RHIBufferHandle buffer,
        const void* data,
        size_t size,
        size_t offset)
    {
        if(!swiftDevice)
            return;
        MetalDevice_updateBuffer(
            swiftDevice,
            handleToID(buffer),
            data,
            size,
            offset
        );
    }

    void MetalDevice::uploadTextureData(
        RHITextureHandle texture,
        const void* data,
        size_t dataSize,
        uint32_t mipLevel,
        uint32_t arraySlice)
    {
        if(!swiftDevice)
            return;
        MetalDevice_uploadTextureData(
            swiftDevice,
            handleToID(texture),
            data,
            dataSize,
            mipLevel,
            arraySlice
        );
    }

    // Resource state transitions (no-op for Metal)
    void MetalDevice::transitionResource(
        RHITextureHandle texture,
        RHIResourceState before,
        RHIResourceState after)
    {
        // No-op for Metal (automatic resource transitions)
    }

    void MetalDevice::transitionResource(
        RHIBufferHandle buffer,
        RHIResourceState before,
        RHIResourceState after)
    {
        // No-op for Metal (automatic resource transitions)
    }

    // Metal-specific
    void* MetalDevice::getSwapchainDrawable(
        RHISwapchainHandle handle)
    {
        if(!swiftDevice)
            return nullptr;
        return MetalDevice_getSwapchainDrawable(
            swiftDevice,
            handleToID(handle)
        );
    }
}

#endif // RHI_METAL
