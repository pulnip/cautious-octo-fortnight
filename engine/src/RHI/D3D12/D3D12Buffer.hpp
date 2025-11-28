#pragma once

#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.hpp"
#include "RHI/RHIDevice.hpp"
#include <cstdint>

// Forward declarations for D3D12
struct ID3D12Resource;
struct ID3D12Device;

namespace RenderToy
{
    struct D3D12Buffer
    {
        ID3D12Resource* buffer = nullptr;
        size_t size = 0;
        RHIBufferUsageFlags usage = RHIBufferUsageFlags::None;
        RHIResourceState currentState = RHIResourceState::Common;

        bool initialize(ID3D12Device* device, const RHIBufferCreateDesc& desc);
        void cleanup();
        void upload(const void* data, size_t dataSize, size_t offset);
    };
}
