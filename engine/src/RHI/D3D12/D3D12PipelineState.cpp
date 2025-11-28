#include "D3D12PipelineState.hpp"
#include "D3D12Shader.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>

namespace RenderToy
{
    bool D3D12PipelineState::initializeGraphics(ID3D12Device* device,
                                               const RHIGraphicsPipelineStateDesc& desc,
                                               D3D12Shader* vertexShader,
                                               D3D12Shader* pixelShader)
    {
        isCompute = false;

        // TODO: Create root signature
        // TODO: Create graphics pipeline state
        LOG_WARN(LOG_RHI, "D3D12PipelineState::initializeGraphics not yet implemented");
        return false;
    }

    bool D3D12PipelineState::initializeCompute(ID3D12Device* device,
                                              const RHIComputePipelineStateDesc& desc,
                                              D3D12Shader* computeShader)
    {
        isCompute = true;

        // TODO: Create root signature
        // TODO: Create compute pipeline state
        LOG_WARN(LOG_RHI, "D3D12PipelineState::initializeCompute not yet implemented");
        return false;
    }

    void D3D12PipelineState::cleanup()
    {
        if (pipelineState) {
            pipelineState->Release();
            pipelineState = nullptr;
        }
        if (rootSignature) {
            rootSignature->Release();
            rootSignature = nullptr;
        }
    }
}

#endif // RHI_D3D12
