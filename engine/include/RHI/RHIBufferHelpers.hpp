#pragma once

#include "RHIDefinitions.hpp"
#include "RHITypes.hpp"
#include "RHIDevice.hpp"
#include <span>
#include <vector>

namespace RenderToy
{
    // Helper functions for creating common buffer types
    namespace RHIBufferHelpers
    {
        // Create a vertex buffer with initial data
        inline RHIBufferHandle createVertexBuffer(
            RHIDevice* device,
            const void* data,
            size_t dataSize,
            uint32_t vertexStride,
            const char* debugName = nullptr)
        {
            RHIBufferCreateDesc desc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            desc.size = dataSize;
            desc.stride = vertexStride;
            desc.usage = RHIBufferUsageFlags::VertexBuffer;
            desc.initialData = data;
            desc.debugName = debugName;

            return device->createBuffer(desc);
        }

        // Create a vertex buffer from a container of vertices
        template<typename T>
        inline RHIBufferHandle createVertexBuffer(
            RHIDevice* device,
            std::span<const T> vertices,
            const char* debugName = nullptr)
        {
            return createVertexBuffer(
                device,
                vertices.data(),
                vertices.size() * sizeof(T),
                sizeof(T),
                debugName
            );
        }

        // Create an index buffer with initial data
        inline RHIBufferHandle createIndexBuffer(
            RHIDevice* device,
            const void* data,
            size_t dataSize,
            RHIIndexFormat format,
            const char* debugName = nullptr)
        {
            RHIBufferCreateDesc desc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            desc.size = dataSize;
            desc.stride = (format == RHIIndexFormat::UInt16) ? 2 : 4;
            desc.usage = RHIBufferUsageFlags::IndexBuffer;
            desc.initialData = data;
            desc.debugName = debugName;

            return device->createBuffer(desc);
        }

        // Create an index buffer from a container of indices
        template<typename T>
        inline RHIBufferHandle createIndexBuffer(
            RHIDevice* device,
            std::span<const T> indices,
            const char* debugName = nullptr)
        {
            static_assert(
                std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
                "Index type must be uint16_t or uint32_t"
            );

            RHIIndexFormat format = std::is_same_v<T, uint16_t>
                ? RHIIndexFormat::UInt16
                : RHIIndexFormat::UInt32;

            return createIndexBuffer(
                device,
                indices.data(),
                indices.size() * sizeof(T),
                format,
                debugName
            );
        }

        // Create a constant buffer
        inline RHIBufferHandle createConstantBuffer(
            RHIDevice* device,
            size_t size,
            const void* initialData = nullptr,
            const char* debugName = nullptr)
        {
            // Constant buffers must be aligned to 256 bytes for D3D12
            constexpr size_t CONSTANT_BUFFER_ALIGNMENT = 256;
            size_t alignedSize = (size + CONSTANT_BUFFER_ALIGNMENT - 1)
                               & ~(CONSTANT_BUFFER_ALIGNMENT - 1);

            RHIBufferCreateDesc desc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            desc.size = alignedSize;
            desc.stride = 0;  // Not applicable for constant buffers
            desc.usage = RHIBufferUsageFlags::ConstantBuffer;
            desc.initialData = initialData;
            desc.debugName = debugName;

            return device->createBuffer(desc);
        }

        // Create a dynamic vertex buffer (CPU writable)
        inline RHIBufferHandle createDynamicVertexBuffer(
            RHIDevice* device,
            size_t size,
            uint32_t vertexStride,
            const char* debugName = nullptr)
        {
            RHIBufferCreateDesc desc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            desc.size = size;
            desc.stride = vertexStride;
            desc.usage = RHIBufferUsageFlags::VertexBuffer | RHIBufferUsageFlags::CPUWrite;
            desc.initialData = nullptr;
            desc.debugName = debugName;

            return device->createBuffer(desc);
        }

        // Create a dynamic index buffer (CPU writable)
        inline RHIBufferHandle createDynamicIndexBuffer(
            RHIDevice* device,
            size_t size,
            RHIIndexFormat format,
            const char* debugName = nullptr)
        {
            RHIBufferCreateDesc desc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            desc.size = size;
            desc.stride = (format == RHIIndexFormat::UInt16) ? 2 : 4;
            desc.usage = RHIBufferUsageFlags::IndexBuffer | RHIBufferUsageFlags::CPUWrite;
            desc.initialData = nullptr;
            desc.debugName = debugName;

            return device->createBuffer(desc);
        }

        // Create a staging buffer for uploading data to GPU
        inline RHIBufferHandle createStagingBuffer(
            RHIDevice* device,
            size_t size,
            const char* debugName = nullptr)
        {
            RHIBufferCreateDesc desc{
                .size = 0,
                .usage = RHIBufferUsageFlags::BufNone,
                .stride = 0,
                .initialData = nullptr,
                .debugName = nullptr
            };
            desc.size = size;
            desc.stride = 0;
            desc.usage = RHIBufferUsageFlags::TransferSrc | RHIBufferUsageFlags::CPUWrite;
            desc.initialData = nullptr;
            desc.debugName = debugName;

            return device->createBuffer(desc);
        }

        // Helper to update buffer data
        inline void updateBuffer(
            RHIDevice* device,
            RHIBufferHandle buffer,
            const void* data,
            size_t dataSize,
            size_t offset = 0)
        {
            device->updateBuffer(buffer, data, dataSize, offset);
        }

        // Template version for type safety
        template<typename T>
        inline void updateBuffer(
            RHIDevice* device,
            RHIBufferHandle buffer,
            std::span<const T> data,
            size_t offset = 0)
        {
            updateBuffer(device, buffer, data.data(), data.size() * sizeof(T), offset);
        }
    }
}
