#pragma once

#include "RHIDefinitions.hpp"
#include "RHITypes.hpp"
#include "RHIDevice.hpp"
#include <array>

namespace RenderToy
{
    // Simplified builder class for constructing graphics pipeline state descriptors
    // Full implementation will be added in Phase 6-8 when actually creating pipelines
    class RHIGraphicsPipelineStateBuilder
    {
    public:
        RHIGraphicsPipelineStateBuilder()
        {
            // Set default values
            desc.topology = RHIPrimitiveTopology::TriangleList;
            desc.renderTargetCount = 1;
            desc.renderTargetFormats[0] = RHITextureFormat::BGRA8_UNORM;
            desc.depthStencilFormat = RHITextureFormat::D32_FLOAT;

            // Default rasterizer state
            desc.rasterizer.fillMode = RHIFillMode::Solid;
            desc.rasterizer.cullMode = RHICullMode::Back;
            desc.rasterizer.frontCounterClockwise = false;

            // Default depth-stencil state
            desc.depthStencil.depthEnable = true;
            desc.depthStencil.depthWriteEnable = true;
            desc.depthStencil.depthFunc = RHIComparisonFunc::Less;

            // Default blend state (no blending)
            desc.blend.renderTargets[0].blendEnable = false;
            desc.blend.renderTargets[0].srcBlend = RHIBlend::One;
            desc.blend.renderTargets[0].destBlend = RHIBlend::Zero;
        }

        // Shader configuration
        RHIGraphicsPipelineStateBuilder& setVertexShader(RHIShaderHandle shader)
        {
            desc.vertexShader = shader;
            return *this;
        }

        RHIGraphicsPipelineStateBuilder& setPixelShader(RHIShaderHandle shader)
        {
            desc.pixelShader = shader;
            return *this;
        }

        // Vertex layout
        RHIGraphicsPipelineStateBuilder& setVertexLayout(const RHIVertexLayout& layout)
        {
            desc.vertexLayout = layout;
            return *this;
        }

        // Primitive topology
        RHIGraphicsPipelineStateBuilder& setPrimitiveTopology(RHIPrimitiveTopology topology)
        {
            desc.topology = topology;
            return *this;
        }

        // Build the descriptor
        const RHIGraphicsPipelineStateDesc& build() const
        {
            return desc;
        }

        // Implicit conversion to descriptor
        operator const RHIGraphicsPipelineStateDesc&() const
        {
            return desc;
        }

    private:
        RHIGraphicsPipelineStateDesc desc;
    };
}
