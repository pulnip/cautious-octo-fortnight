#pragma once

#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.h"

namespace RenderToy
{
    // CPU-side shader resource
    // Holds reference to GPU compiled shader
    struct Shader{
        RHIShaderHandle rhiShader;  // GPU shader handle
        RHIShaderStage stage = RHIShaderStage_Vertex;

        inline bool isValid() const {
            return rhiShader.isValid();
        }
    };
}
