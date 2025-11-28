#pragma once

#include "RHI/RHIResource.hpp"
#include "RHI/RHIDefinitions.hpp"
#include <vector>

namespace RenderToy
{
    // Compiled shader object (vertex, pixel, compute)
    class RHIShader : public RHIResource
    {
    protected:
        RHIShaderStage stage;
        std::vector<uint8_t> bytecode; // Cached bytecode

    public:
        RHIShader(RHIShaderStage stage, const void* bytecode, size_t bytecodeSize)
            : stage(stage)
            , bytecode(static_cast<const uint8_t*>(bytecode),
                      static_cast<const uint8_t*>(bytecode) + bytecodeSize) {}

        virtual ~RHIShader() = default;

        RHIShaderStage getStage() const { return stage; }
        const void* getBytecode() const { return bytecode.data(); }
        size_t getBytecodeSize() const { return bytecode.size(); }

        // Platform-specific shader object getter
        virtual void* getNativeShader() = 0;
    };
}
