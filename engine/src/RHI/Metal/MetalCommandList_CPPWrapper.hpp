#pragma once

#include "RHI/RHICommandList.hpp"
#include "MetalTypes.h"

namespace RenderToy
{
    class MetalDevice;

    // C++ wrapper for Swift MetalCommandList implementation
    class MetalCommandList final: public RHICommandList{
        friend class MetalDevice;

    public:
        MetalCommandList(
            MetalCommandListPtr handle,
            MetalDevice* device);
        ~MetalCommandList() override;

        // Command list lifecycle
        void begin() override;
        void close() override;
        void reset() override;

        // Render pass control
        void beginRenderPass(
            RHITextureHandle renderTarget,
            RHITextureHandle depthStencil = {},
            RHILoadAction loadAction = RHILoadAction_Load,
            RHIStoreAction storeAction = RHIStoreAction_Store,
            const RHIClearColor& clearColor = {
                .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f
            }) override;

        void beginRenderPass(
            const RHITextureHandle* renderTargets,
            uint32_t renderTargetCount,
            RHITextureHandle depthStencil = {},
            RHILoadAction loadAction = RHILoadAction_Load,
            RHIStoreAction storeAction = RHIStoreAction_Store,
            const RHIClearColor* clearColors = nullptr) override;

        void endRenderPass() override;

        // Clear operations
        void clearRenderTarget(
            RHITextureHandle renderTarget,
            const RHIClearColor& color) override;

        void clearDepthStencil(
            RHITextureHandle depthStencil,
            float depth,
            uint8_t stencil = 0) override;

        // Pipeline state
        void setPipelineState(RHIPipelineStateHandle pso) override;

        // Vertex and index buffers
        void setVertexBuffer(
            uint32_t slot,
            RHIBufferHandle buffer,
            uint32_t stride,
            uint32_t offset = 0) override;

        void setIndexBuffer(
            RHIBufferHandle buffer,
            RHIIndexFormat format,
            uint32_t offset = 0) override;

        // Constant buffers
        void setConstantBuffer(
            uint32_t slot,
            RHIBufferHandle buffer,
            RHIShaderStage stage) override;

        // Shader resources
        void setTexture(
            uint32_t slot,
            RHITextureHandle texture,
            RHIShaderStage stage) override;

        void setBuffer(
            uint32_t slot,
            RHIBufferHandle buffer,
            RHIShaderStage stage) override;

        // Viewport and scissor
        void setViewport(const RHIViewport& viewport) override;
        void setViewports(const RHIViewport* viewports, uint32_t count) override;
        void setScissorRect(const RHIScissorRect& scissor) override;
        void setScissorRects(const RHIScissorRect* scissors, uint32_t count) override;

        // Draw commands
        void draw(
            uint32_t vertexCount,
            uint32_t instanceCount = 1,
            uint32_t startVertex = 0,
            uint32_t startInstance = 0) override;

        void drawIndexed(
            uint32_t indexCount,
            uint32_t instanceCount = 1,
            uint32_t startIndex = 0,
            int32_t  baseVertex = 0,
            uint32_t startInstance = 0) override;

        // Compute dispatch
        void dispatch(
            uint32_t threadGroupCountX,
            uint32_t threadGroupCountY,
            uint32_t threadGroupCountZ) override;

        // Resource barriers (no-op for Metal)
        void transitionBarrier(
            RHITextureHandle texture,
            RHIResourceState before,
            RHIResourceState after) override;

        void transitionBarrier(
            RHIBufferHandle buffer,
            RHIResourceState before,
            RHIResourceState after) override;

        // Copy operations
        void copyBuffer(
            RHIBufferHandle src,
            RHIBufferHandle dst,
            size_t srcOffset,
            size_t dstOffset,
            size_t size) override;

        void copyTexture(
            RHITextureHandle src,
            RHITextureHandle dst) override;

        void copyBufferToTexture(
            RHIBufferHandle src,
            RHITextureHandle dst,
            uint32_t mipLevel = 0,
            uint32_t arraySlice = 0) override;

        // Debug markers
        void beginEvent(const char* name) override;
        void endEvent() override;
        void setMarker(const char* name) override;

        // Metal-specific: Set drawable
        void setDrawable(void* drawable);

        // Accessor for device wrapper
        inline MetalCommandListPtr getSwiftCommandList() const { return swiftCmdList; }

    private:
        MetalCommandListPtr swiftCmdList; // Opaque pointer to Swift MetalCommandList
        MetalDevice* device; // Parent device
    };
}
