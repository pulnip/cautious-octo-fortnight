#include "RHI/RHIDevice.hpp"
#include "RHI/RHIPlatform.h"
#include "Log/Log.hpp"

namespace RenderToy
{
    // Platform-specific device implementations
#if defined(RHI_D3D12)
    // Forward declaration - will be implemented in D3D12Device.cpp
    std::unique_ptr<RHIDevice> createD3D12Device(const RHIDeviceCreateDesc& desc);
#elif defined(RHI_METAL)
    // Forward declaration - will be implemented in Metal/MetalDeviceFactory.cpp
    std::unique_ptr<RHIDevice> createMetalDevice(const RHIDeviceCreateDesc& desc);
#endif

    std::unique_ptr<RHIDevice> RHIDevice::create(const RHIDeviceCreateDesc& desc){
        LOG_INFO(LOG_RHI, "Creating RHI device...");
        LOG_INFO(LOG_RHI, "  Application: {}", desc.applicationName);
        LOG_INFO(LOG_RHI, "  Debug layer: {}", desc.enableDebugLayer ? "enabled" : "disabled");
        LOG_INFO(LOG_RHI, "  GPU validation: {}", desc.enableGPUValidation ? "enabled" : "disabled");

#if defined(RHI_D3D12)
        LOG_INFO(LOG_RHI, "Using DirectX 12 backend");
        return createD3D12Device(desc);
#elif defined(RHI_METAL)
        LOG_INFO(LOG_RHI, "Using Metal backend");
        return createMetalDevice(desc);
#else
        #error "No RHI backend available. Define RHI_D3D12 or RHI_METAL."
        return nullptr;
#endif
    }
}
