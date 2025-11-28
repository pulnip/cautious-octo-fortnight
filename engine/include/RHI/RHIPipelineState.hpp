#pragma once

#include "RHI/RHIResource.hpp"
#include "RHI/RHIDefinitions.hpp"

namespace RenderToy
{
    // Immutable pipeline state object (graphics or compute)
    // Encapsulates shaders, rasterizer state, blend state, depth/stencil state
    class RHIPipelineState : public RHIResource
    {
    protected:
        bool isCompute;

    public:
        RHIPipelineState(bool isCompute = false)
            : isCompute(isCompute) {}

        virtual ~RHIPipelineState() = default;

        bool isComputePipeline() const { return isCompute; }
        bool isGraphicsPipeline() const { return !isCompute; }

        // Platform-specific PSO getter
        virtual void* getNativePipelineState() = 0;
    };
}
