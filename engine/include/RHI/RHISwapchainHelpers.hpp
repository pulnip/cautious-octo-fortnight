#pragma once

#include "RHISwapchain.hpp"
#include "RHIDevice.hpp"
#include "RHITypes.hpp"
#include "Log/Category.hpp"
#include "Log/Log.hpp"

namespace RenderToy
{
    // Helper functions for swapchain management
    namespace RHISwapchainHelpers
    {
        // Create a swapchain with sensible defaults
        inline RHISwapchainHandle createSwapchain(
            RHIDevice* device,
            void* windowHandle,
            uint32_t width,
            uint32_t height,
            RHITextureFormat format = RHITextureFormat::BGRA8_UNORM,
            uint32_t bufferCount = 3,
            bool vsync = true)
        {
            RHISwapchainCreateDesc desc{};
            desc.windowHandle = windowHandle;
            desc.width = width;
            desc.height = height;
            desc.format = format;
            desc.bufferCount = bufferCount;
            desc.vsync = vsync;

            RHISwapchainHandle swapchain = device->createSwapchain(desc);
            if (swapchain.isValid()) {
                LOG_INFO(LOG_RHI, "Created swapchain: {}x{}, format={}, buffers={}, vsync={}",
                         width, height, static_cast<int>(format), bufferCount, vsync);
            }
            else {
                LOG_ERROR(LOG_RHI, "Failed to create swapchain");
            }

            return swapchain;
        }

        // Resize swapchain
        inline bool resizeSwapchain(
            RHIDevice* device,
            RHISwapchainHandle swapchain,
            uint32_t newWidth,
            uint32_t newHeight)
        {
            if (!swapchain.isValid()) {
                LOG_ERROR(LOG_RHI, "Cannot resize invalid swapchain");
                return false;
            }

            if (newWidth == 0 || newHeight == 0) {
                LOG_WARN(LOG_RHI, "Invalid swapchain dimensions: {}x{}", newWidth, newHeight);
                return false;
            }

            bool result = device->resizeSwapchain(swapchain, newWidth, newHeight);
            if (result) {
                LOG_INFO(LOG_RHI, "Resized swapchain to {}x{}", newWidth, newHeight);
            }
            else {
                LOG_ERROR(LOG_RHI, "Failed to resize swapchain to {}x{}", newWidth, newHeight);
            }

            return result;
        }

        // Present with error handling
        inline bool present(RHIDevice* device, RHISwapchainHandle swapchain)
        {
            if (!swapchain.isValid()) {
                LOG_ERROR(LOG_RHI, "Cannot present with invalid swapchain");
                return false;
            }

            bool result = device->present(swapchain);
            if (!result) {
                LOG_ERROR(LOG_RHI, "Present failed");
            }

            return result;
        }

        // Get current backbuffer
        inline RHITextureHandle getCurrentBackbuffer(
            RHIDevice* device,
            RHISwapchainHandle swapchain)
        {
            if (!swapchain.isValid()) {
                LOG_ERROR(LOG_RHI, "Cannot get backbuffer from invalid swapchain");
                return RHITextureHandle{};
            }

            return device->getSwapchainBackbuffer(swapchain);
        }

        // Get backbuffer index
        inline uint32_t getCurrentBackbufferIndex(
            RHIDevice* device,
            RHISwapchainHandle swapchain)
        {
            if (!swapchain.isValid()) {
                LOG_ERROR(LOG_RHI, "Cannot get backbuffer index from invalid swapchain");
                return 0;
            }

            return device->getSwapchainCurrentIndex(swapchain);
        }
    }

    // Swapchain wrapper class for easier management
    class RHISwapchainWrapper
    {
    public:
        RHISwapchainWrapper() = default;

        RHISwapchainWrapper(
            RHIDevice* device,
            void* windowHandle,
            uint32_t width,
            uint32_t height,
            RHITextureFormat format = RHITextureFormat::BGRA8_UNORM,
            uint32_t bufferCount = 3,
            bool vsync = true)
            : device(device)
            , width(width)
            , height(height)
            , format(format)
            , vsync(vsync)
        {
            swapchain = RHISwapchainHelpers::createSwapchain(
                device, windowHandle, width, height, format, bufferCount, vsync);
        }

        ~RHISwapchainWrapper()
        {
            if (swapchain.isValid() && device) {
                device->destroySwapchain(swapchain);
            }
        }

        // No copy
        RHISwapchainWrapper(const RHISwapchainWrapper&) = delete;
        RHISwapchainWrapper& operator=(const RHISwapchainWrapper&) = delete;

        // Move support
        RHISwapchainWrapper(RHISwapchainWrapper&& other) noexcept
            : device(other.device)
            , swapchain(other.swapchain)
            , width(other.width)
            , height(other.height)
            , format(other.format)
            , vsync(other.vsync)
        {
            other.swapchain = RHISwapchainHandle{};
        }

        RHISwapchainWrapper& operator=(RHISwapchainWrapper&& other) noexcept
        {
            if (this != &other) {
                if (swapchain.isValid() && device) {
                    device->destroySwapchain(swapchain);
                }

                device = other.device;
                swapchain = other.swapchain;
                width = other.width;
                height = other.height;
                format = other.format;
                vsync = other.vsync;

                other.swapchain = RHISwapchainHandle{};
            }
            return *this;
        }

        bool resize(uint32_t newWidth, uint32_t newHeight)
        {
            if (RHISwapchainHelpers::resizeSwapchain(device, swapchain, newWidth, newHeight)) {
                width = newWidth;
                height = newHeight;
                return true;
            }
            return false;
        }

        bool present()
        {
            return RHISwapchainHelpers::present(device, swapchain);
        }

        RHITextureHandle getCurrentBackbuffer() const
        {
            return RHISwapchainHelpers::getCurrentBackbuffer(device, swapchain);
        }

        uint32_t getCurrentBackbufferIndex() const
        {
            return RHISwapchainHelpers::getCurrentBackbufferIndex(device, swapchain);
        }

        RHISwapchainHandle getHandle() const { return swapchain; }
        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        RHITextureFormat getFormat() const { return format; }
        bool isVSyncEnabled() const { return vsync; }
        bool isValid() const { return swapchain.isValid(); }

    private:
        RHIDevice* device = nullptr;
        RHISwapchainHandle swapchain;
        uint32_t width = 0;
        uint32_t height = 0;
        RHITextureFormat format = RHITextureFormat::Unknown;
        bool vsync = true;
    };
}
