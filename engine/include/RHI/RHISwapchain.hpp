#pragma once

#include "RHI/RHIResource.hpp"
#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.h"

namespace RenderToy
{
    // Swapchain for presenting rendered images to the screen
    class RHISwapchain : public RHIResource
    {
    protected:
        uint32_t width;
        uint32_t height;
        uint32_t bufferCount;
        RHITextureFormat format;
        uint32_t currentBufferIndex;

    public:
        RHISwapchain(uint32_t width, uint32_t height,
                    uint32_t bufferCount, RHITextureFormat format)
            : width(width), height(height)
            , bufferCount(bufferCount), format(format)
            , currentBufferIndex(0) {}

        virtual ~RHISwapchain() = default;

        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        uint32_t getBufferCount() const { return bufferCount; }
        RHITextureFormat getFormat() const { return format; }
        uint32_t getCurrentBufferIndex() const { return currentBufferIndex; }

        // Get current backbuffer for rendering
        virtual RHITextureHandle getCurrentBackbuffer() = 0;

        // Present rendered image to screen
        virtual void present(bool vsync = true) = 0;

        // Resize swapchain (window resize)
        virtual void resize(uint32_t newWidth, uint32_t newHeight) = 0;

        // Platform-specific swapchain getter
        virtual void* getNativeSwapchain() = 0;
    };
}
