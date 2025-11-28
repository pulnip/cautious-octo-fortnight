#pragma once

#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.h"

namespace RenderToy
{
    // Command list for recording GPU commands
    // Commands are deferred until submission via RHIDevice::submitCommandList()
    class RHICommandList
    {
    public:
        virtual ~RHICommandList() = default;

        // Command list lifecycle
        virtual void begin() = 0;
        virtual void close() = 0;
        virtual void reset() = 0;

        // Render pass control
        virtual void beginRenderPass(
            RHITextureHandle renderTarget,
            RHITextureHandle depthStencil = {},
            RHILoadAction loadAction = RHILoadAction_Load,
            RHIStoreAction storeAction = RHIStoreAction_Store,
            const RHIClearColor& clearColor = {
                .r=0.0f, .g=0.0f, .b=0.0f, .a=1.0f
            }) = 0;

        virtual void beginRenderPass(
            const RHITextureHandle* renderTargets,
            uint32_t renderTargetCount,
            RHITextureHandle depthStencil = {},
            RHILoadAction loadAction = RHILoadAction_Load,
            RHIStoreAction storeAction = RHIStoreAction_Store,
            const RHIClearColor* clearColors = nullptr) = 0;

        virtual void endRenderPass() = 0;

        // Clear operations
        virtual void clearRenderTarget(
            RHITextureHandle renderTarget,
            const RHIClearColor& color) = 0;

        virtual void clearDepthStencil(
            RHITextureHandle depthStencil,
            float depth,
            uint8_t stencil = 0) = 0;

        // Pipeline state
        virtual void setPipelineState(
            RHIPipelineStateHandle pso) = 0;

        // Vertex and index buffers
        virtual void setVertexBuffer(
            uint32_t slot,
            RHIBufferHandle buffer,
            uint32_t stride,
            uint32_t offset = 0) = 0;

        virtual void setIndexBuffer(
            RHIBufferHandle buffer,
            RHIIndexFormat format,
            uint32_t offset = 0) = 0;

        // Constant buffers
        virtual void setConstantBuffer(
            uint32_t slot,
            RHIBufferHandle buffer,
            RHIShaderStage stage) = 0;

        // Shader resources (textures, buffers)
        virtual void setTexture(
            uint32_t slot,
            RHITextureHandle texture,
            RHIShaderStage stage) = 0;

        virtual void setBuffer(
            uint32_t slot,
            RHIBufferHandle buffer,
            RHIShaderStage stage) = 0;

        // Viewport and scissor
        virtual void setViewport(
            const RHIViewport& viewport) = 0;
        virtual void setViewports(
            const RHIViewport* viewports,
            uint32_t count) = 0;
        virtual void setScissorRect(
            const RHIScissorRect& scissor) = 0;
        virtual void setScissorRects(
            const RHIScissorRect* scissors,
            uint32_t count) = 0;

        // Draw commands
        virtual void draw(
            uint32_t vertexCount,
            uint32_t instanceCount = 1,
            uint32_t startVertex = 0,
            uint32_t startInstance = 0) = 0;

        virtual void drawIndexed(
            uint32_t indexCount,
            uint32_t instanceCount = 1,
            uint32_t startIndex = 0,
            int32_t baseVertex = 0,
            uint32_t startInstance = 0) = 0;

        // Compute dispatch
        virtual void dispatch(
            uint32_t threadGroupCountX,
            uint32_t threadGroupCountY,
            uint32_t threadGroupCountZ) = 0;

        // Resource barriers (state transitions)
        virtual void transitionBarrier(
            RHITextureHandle texture,
            RHIResourceState before,
            RHIResourceState after) = 0;

        virtual void transitionBarrier(
            RHIBufferHandle buffer,
            RHIResourceState before,
            RHIResourceState after) = 0;

        // Copy operations
        virtual void copyBuffer(
            RHIBufferHandle src,
            RHIBufferHandle dst,
            size_t srcOffset,
            size_t dstOffset,
            size_t size) = 0;

        virtual void copyTexture(
            RHITextureHandle src,
            RHITextureHandle dst) = 0;

        virtual void copyBufferToTexture(
            RHIBufferHandle src,
            RHITextureHandle dst,
            uint32_t mipLevel = 0,
            uint32_t arraySlice = 0) = 0;

        // Debug markers (for GPU profiling)
        virtual void beginEvent(const char* name) = 0;
        virtual void endEvent() = 0;
        virtual void setMarker(const char* name) = 0;

    protected:
        // Only RHIDevice can create command lists
        RHICommandList() = default;
    };
}
