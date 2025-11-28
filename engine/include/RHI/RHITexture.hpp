#pragma once

#include "RHI/RHIResource.hpp"
#include "RHI/RHIDefinitions.h"

namespace RenderToy
{
    // GPU texture resource (1D, 2D, 3D, Cube, arrays)
    class RHITexture : public RHIResource
    {
    protected:
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        uint32_t arraySize;
        RHITextureFormat format;
        RHITextureUsageFlags usage;

    public:
        RHITexture(uint32_t width, uint32_t height, uint32_t depth,
                  uint32_t mipLevels, uint32_t arraySize,
                  RHITextureFormat format, RHITextureUsageFlags usage)
            : width(width), height(height), depth(depth)
            , mipLevels(mipLevels), arraySize(arraySize)
            , format(format), usage(usage) {}

        virtual ~RHITexture() = default;

        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        uint32_t getDepth() const { return depth; }
        uint32_t getMipLevels() const { return mipLevels; }
        uint32_t getArraySize() const { return arraySize; }
        RHITextureFormat getFormat() const { return format; }
        RHITextureUsageFlags getUsage() const { return usage; }

        bool is2D() const { return depth == 1 && arraySize == 1; }
        bool is3D() const { return depth > 1; }
        bool isArray() const { return arraySize > 1; }
        bool isCube() const { return arraySize == 6; }

        // Platform-specific resource getter (for interop)
        virtual void* getNativeResource() = 0;
    };
}
