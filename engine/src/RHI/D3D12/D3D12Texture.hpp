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
    struct D3D12Texture
    {
        ID3D12Resource* texture = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
        uint32_t mipLevels = 1;
        uint32_t arraySize = 1;
        RHITextureFormat format = RHITextureFormat::Unknown;
        RHITextureUsageFlags usage = RHITextureUsageFlags::None;
        RHIResourceState currentState = RHIResourceState::Common;

        bool initialize(ID3D12Device* device, const RHITextureCreateDesc& desc);
        void cleanup();
        void upload(const void* data, size_t dataSize, uint32_t mipLevel, uint32_t arraySlice);
    };
}
