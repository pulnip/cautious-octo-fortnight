#include "D3D12Shader.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>

namespace RenderToy
{
    bool D3D12Shader::initialize(ID3D12Device* device, const RHIShaderCreateDesc& desc)
    {
        stage = desc.stage;

        // Copy bytecode (D3D12 expects precompiled DXIL bytecode)
        bytecodeData.resize(desc.bytecodeSize);
        memcpy(bytecodeData.data(), desc.bytecode, desc.bytecodeSize);

        LOG_INFO(LOG_RHI, "D3D12 shader created: {} bytes", desc.bytecodeSize);
        return true;
    }

    void D3D12Shader::cleanup()
    {
        if (bytecode) {
            bytecode->Release();
            bytecode = nullptr;
        }
        bytecodeData.clear();
    }
}

#endif // RHI_D3D12
