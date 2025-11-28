#pragma once

#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.hpp"
#include "RHI/RHIDevice.hpp"
#include <cstdint>

// Forward declarations for D3D12
struct ID3D12PipelineState;
struct ID3D12RootSignature;
struct ID3D12Device;

namespace RenderToy
{
    struct D3D12Shader;

    struct D3D12PipelineState
    {
        ID3D12PipelineState* pipelineState = nullptr;
        ID3D12RootSignature* rootSignature = nullptr;
        bool isCompute = false;

        bool initializeGraphics(ID3D12Device* device,
                               const RHIGraphicsPipelineStateDesc& desc,
                               D3D12Shader* vertexShader,
                               D3D12Shader* pixelShader);

        bool initializeCompute(ID3D12Device* device,
                              const RHIComputePipelineStateDesc& desc,
                              D3D12Shader* computeShader);

        void cleanup();
    };
}
