#pragma once

#include "RHI/RHIResource.hpp"
#include "RHI/RHIDefinitions.h"

namespace RenderToy
{
    // GPU buffer resource (vertex, index, constant, structured buffers)
    class RHIBuffer: public RHIResource{
    protected:
        size_t size;
        RHIBufferUsageFlags usage;
        uint32_t stride;

    public:
        RHIBuffer(size_t size, RHIBufferUsageFlags usage, uint32_t stride = 0)
            :size(size), usage(usage), stride(stride){}

        virtual ~RHIBuffer() = default;

        inline size_t getSize() const{ return size; }
        inline RHIBufferUsageFlags getUsage() const{ return usage; }
        inline uint32_t getStride() const{ return stride; }

        // Map/unmap for CPU access (for constant buffers, staging buffers)
        virtual void* map() = 0;
        virtual void unmap() = 0;

        // Platform-specific resource getter (for interop)
        virtual void* getNativeResource() = 0;
    };
}
