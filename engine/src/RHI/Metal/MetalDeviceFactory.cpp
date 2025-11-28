#include "RHI/RHIDevice.hpp"
#include "RHI/RHIPlatform.h"
#include "Log/Log.hpp"
#include "MetalDevice_CPPWrapper.hpp"

#if RHI_METAL

namespace RenderToy
{
    std::unique_ptr<RHIDevice> createMetalDevice(const RHIDeviceCreateDesc& desc)
    {
        auto device = std::make_unique<MetalDevice>();

        if (!device->initialize(desc)) {
            LOG_ERROR(LOG_METAL, "Failed to initialize Metal device");
            return nullptr;
        }

        return device;
    }
}

#endif // RHI_METAL
