#include "D3D12CommandList.hpp"
#include "D3D12Device.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>

namespace RenderToy
{
    D3D12CommandList::D3D12CommandList(ID3D12CommandAllocator* allocator, D3D12Device* dev)
        : commandAllocator(allocator)
        , commandList(nullptr)
        , device(dev)
        , isRecording(false)
        , currentRenderTargetCount(0)
    {
        for (uint32_t i = 0; i < RHI_MAX_RENDER_TARGETS; ++i) {
            currentRenderTargets[i] = RHITextureHandle{};
        }
    }

    void D3D12CommandList::begin()
    {
        if (isRecording) {
            LOG_WARN(LOG_RHI, "Command list already recording");
            return;
        }

        // Create command list
        if (!commandList) {
            device->getD3D12Device()->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                commandAllocator,
                nullptr,
                IID_PPV_ARGS(&commandList)
            );
        } else {
            commandList->Reset(commandAllocator, nullptr);
        }

        isRecording = true;
    }

    void D3D12CommandList::close()
    {
        if (!isRecording) {
            LOG_WARN(LOG_RHI, "Command list not recording");
            return;
        }

        commandList->Close();
        isRecording = false;
    }

    void D3D12CommandList::reset()
    {
        if (isRecording) {
            LOG_WARN(LOG_RHI, "Cannot reset while recording");
            return;
        }

        commandList->Reset(commandAllocator, nullptr);
    }

    void D3D12CommandList::beginRenderPass(RHITextureHandle renderTarget,
                                          RHITextureHandle depthStencil,
                                          RHILoadAction loadAction,
                                          RHIStoreAction storeAction,
                                          const RHIClearColor& clearColor)
    {
        // TODO: Implement D3D12 render pass
        LOG_WARN(LOG_RHI, "D3D12CommandList::beginRenderPass not yet implemented");
    }

    void D3D12CommandList::beginRenderPass(const RHITextureHandle* renderTargets,
                                          uint32_t renderTargetCount,
                                          RHITextureHandle depthStencil,
                                          RHILoadAction loadAction,
                                          RHIStoreAction storeAction,
                                          const RHIClearColor* clearColors)
    {
        // TODO: Implement D3D12 render pass
        LOG_WARN(LOG_RHI, "D3D12CommandList::beginRenderPass (MRT) not yet implemented");
    }

    void D3D12CommandList::endRenderPass()
    {
        // TODO: Implement D3D12 render pass
    }

    void D3D12CommandList::clearRenderTarget(RHITextureHandle renderTarget,
                                            const RHIClearColor& color)
    {
        // TODO: Implement
        LOG_WARN(LOG_RHI, "D3D12CommandList::clearRenderTarget not yet implemented");
    }

    void D3D12CommandList::clearDepthStencil(RHITextureHandle depthStencil,
                                            float depth,
                                            uint8_t stencil)
    {
        // TODO: Implement
        LOG_WARN(LOG_RHI, "D3D12CommandList::clearDepthStencil not yet implemented");
    }

    void D3D12CommandList::setPipelineState(RHIPipelineStateHandle pso)
    {
        // TODO: Implement
        LOG_WARN(LOG_RHI, "D3D12CommandList::setPipelineState not yet implemented");
    }

    void D3D12CommandList::setVertexBuffer(uint32_t slot, RHIBufferHandle buffer,
                                          uint32_t stride, uint32_t offset)
    {
        // TODO: Implement
    }

    void D3D12CommandList::setIndexBuffer(RHIBufferHandle buffer,
                                         RHIIndexFormat format, uint32_t offset)
    {
        // TODO: Implement
    }

    void D3D12CommandList::setConstantBuffer(uint32_t slot, RHIBufferHandle buffer,
                                            RHIShaderStage stage)
    {
        // TODO: Implement
    }

    void D3D12CommandList::setTexture(uint32_t slot, RHITextureHandle texture,
                                     RHIShaderStage stage)
    {
        // TODO: Implement
    }

    void D3D12CommandList::setBuffer(uint32_t slot, RHIBufferHandle buffer,
                                    RHIShaderStage stage)
    {
        // TODO: Implement
    }

    void D3D12CommandList::setViewport(const RHIViewport& viewport)
    {
        if (!commandList) return;

        D3D12_VIEWPORT vp;
        vp.TopLeftX = viewport.x;
        vp.TopLeftY = viewport.y;
        vp.Width = viewport.width;
        vp.Height = viewport.height;
        vp.MinDepth = viewport.minDepth;
        vp.MaxDepth = viewport.maxDepth;

        commandList->RSSetViewports(1, &vp);
    }

    void D3D12CommandList::setViewports(const RHIViewport* viewports, uint32_t count)
    {
        if (count > 0) {
            setViewport(viewports[0]);
        }
    }

    void D3D12CommandList::setScissorRect(const RHIScissorRect& scissor)
    {
        if (!commandList) return;

        D3D12_RECT rect;
        rect.left = scissor.left;
        rect.top = scissor.top;
        rect.right = scissor.right;
        rect.bottom = scissor.bottom;

        commandList->RSSetScissorRects(1, &rect);
    }

    void D3D12CommandList::setScissorRects(const RHIScissorRect* scissors, uint32_t count)
    {
        if (count > 0) {
            setScissorRect(scissors[0]);
        }
    }

    void D3D12CommandList::draw(uint32_t vertexCount, uint32_t instanceCount,
                               uint32_t startVertex, uint32_t startInstance)
    {
        if (!commandList) return;
        commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
    }

    void D3D12CommandList::drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                      uint32_t startIndex, int32_t baseVertex,
                                      uint32_t startInstance)
    {
        if (!commandList) return;
        commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex,
                                         baseVertex, startInstance);
    }

    void D3D12CommandList::dispatch(uint32_t threadGroupCountX,
                                   uint32_t threadGroupCountY,
                                   uint32_t threadGroupCountZ)
    {
        if (!commandList) return;
        commandList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
    }

    void D3D12CommandList::transitionBarrier(RHITextureHandle texture,
                                           RHIResourceState before,
                                           RHIResourceState after)
    {
        // TODO: Implement resource barriers
        LOG_WARN(LOG_RHI, "D3D12CommandList::transitionBarrier (texture) not yet implemented");
    }

    void D3D12CommandList::transitionBarrier(RHIBufferHandle buffer,
                                           RHIResourceState before,
                                           RHIResourceState after)
    {
        // TODO: Implement resource barriers
    }

    void D3D12CommandList::copyBuffer(RHIBufferHandle src, RHIBufferHandle dst,
                                     size_t srcOffset, size_t dstOffset, size_t size)
    {
        // TODO: Implement
        LOG_WARN(LOG_RHI, "D3D12CommandList::copyBuffer not yet implemented");
    }

    void D3D12CommandList::copyTexture(RHITextureHandle src, RHITextureHandle dst)
    {
        // TODO: Implement
        LOG_WARN(LOG_RHI, "D3D12CommandList::copyTexture not yet implemented");
    }

    void D3D12CommandList::copyBufferToTexture(RHIBufferHandle src, RHITextureHandle dst,
                                              uint32_t mipLevel, uint32_t arraySlice)
    {
        // TODO: Implement
        LOG_WARN(LOG_RHI, "D3D12CommandList::copyBufferToTexture not yet implemented");
    }

    void D3D12CommandList::beginEvent(const char* name)
    {
        // TODO: Implement PIX markers
    }

    void D3D12CommandList::endEvent()
    {
        // TODO: Implement PIX markers
    }

    void D3D12CommandList::setMarker(const char* name)
    {
        // TODO: Implement PIX markers
    }
}

#endif // RHI_D3D12
