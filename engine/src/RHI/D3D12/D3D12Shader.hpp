#pragma once

#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.hpp"
#include "RHI/RHIDevice.hpp"
#include <cstdint>
#include <vector>

// Forward declarations for D3D12
struct ID3D12Device;
struct ID3DBlob;

namespace RenderToy
{
    struct D3D12Shader
    {
        ID3DBlob* bytecode = nullptr;
        RHIShaderStage stage = RHIShaderStage::Vertex;
        std::vector<uint8_t> bytecodeData;  // Store bytecode for later use

        bool initialize(ID3D12Device* device, const RHIShaderCreateDesc& desc);
        void cleanup();
    };
}
