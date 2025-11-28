#include "MetalCommandList_CPPWrapper.hpp"
#include "MetalDevice_CPPWrapper.hpp"
#include "MetalCommandList_Bridge.h"
#include <cstdlib>  // for alloca on macOS

#if RHI_METAL

namespace RenderToy
{
    // Helper functions to convert between generic_handle and uint64_t
    template<typename T>
    static uint64_t handleToID(generic_handle<T> handle){
        return handle.index;
    }

    template<typename T>
    static generic_handle<T> idToHandle(uint64_t id){
        generic_handle<T> handle;
        handle.index = id;
        handle.generation = 0;  // Generation not used in Swift bridge
        return handle;
    }

    MetalCommandList::MetalCommandList(MetalCommandListPtr handle, MetalDevice* dev)
        : swiftCmdList(handle), device(dev)
    {
    }

    MetalCommandList::~MetalCommandList(){
        if (swiftCmdList) {
            MetalCommandList_destroy(swiftCmdList);
            swiftCmdList = nullptr;
        }
    }

    void MetalCommandList::begin(){
        if (!swiftCmdList) return;
        MetalCommandList_begin(swiftCmdList);
    }

    void MetalCommandList::close(){
        if (!swiftCmdList) return;
        MetalCommandList_close(swiftCmdList);
    }

    void MetalCommandList::reset(){
        if (!swiftCmdList) return;
        MetalCommandList_reset(swiftCmdList);
    }

    void MetalCommandList::beginRenderPass(
        RHITextureHandle renderTarget,
        RHITextureHandle depthStencil,
        RHILoadAction loadAction,
        RHIStoreAction storeAction,
        const RHIClearColor& clearColor)
    {
        if (!swiftCmdList) return;
        MetalCommandList_beginRenderPass(
            swiftCmdList,
            handleToID(renderTarget),
            handleToID(depthStencil),
            static_cast<::RHILoadAction>(loadAction),
            static_cast<::RHIStoreAction>(storeAction),
            &clearColor
        );
    }

    void MetalCommandList::beginRenderPass(
        const RHITextureHandle* renderTargets,
        uint32_t renderTargetCount,
        RHITextureHandle depthStencil,
        RHILoadAction loadAction,
        RHIStoreAction storeAction,
        const RHIClearColor* clearColors)
    {
        if (!swiftCmdList) return;

        // Convert handle array to uint64_t array
        uint64_t* rtIDs = (uint64_t*)alloca(renderTargetCount * sizeof(uint64_t));
        for(uint32_t i = 0; i < renderTargetCount; ++i){
            rtIDs[i] = handleToID(renderTargets[i]);
        }

        MetalCommandList_beginRenderPassMultiTarget(
            swiftCmdList,
            rtIDs,
            renderTargetCount,
            handleToID(depthStencil),
            static_cast<::RHILoadAction>(loadAction),
            static_cast<::RHIStoreAction>(storeAction),
            clearColors
        );
    }

    void MetalCommandList::endRenderPass(){
        if(!swiftCmdList)
            return;
        MetalCommandList_endRenderPass(swiftCmdList);
    }

    void MetalCommandList::clearRenderTarget(
        RHITextureHandle renderTarget,
        const RHIClearColor& color)
    {
        // Metal doesn't have a separate clear command - it's done via load action
        // This is a no-op
    }

    void MetalCommandList::clearDepthStencil(
        RHITextureHandle depthStencil,
        float depth,
        uint8_t stencil)
    {
        // Metal doesn't have a separate clear command - it's done via load action
        // This is a no-op
    }

    void MetalCommandList::setPipelineState(RHIPipelineStateHandle pso){
        if(!swiftCmdList)
            return;
        MetalCommandList_setPipelineState(
            swiftCmdList,
            handleToID(pso)
        );
    }

    void MetalCommandList::setVertexBuffer(
        uint32_t slot,
        RHIBufferHandle buffer,
        uint32_t stride,
        uint32_t offset)
    {
        if (!swiftCmdList)
            return;
        MetalCommandList_setVertexBuffer(
            swiftCmdList,
            slot,
            handleToID(buffer),
            stride,
            offset
        );
    }

    void MetalCommandList::setIndexBuffer(
        RHIBufferHandle buffer,
        RHIIndexFormat format,
        uint32_t offset)
    {
        if (!swiftCmdList)
            return;
        MetalCommandList_setIndexBuffer(
            swiftCmdList,
            handleToID(buffer),
            static_cast<::RHIIndexFormat>(format),
            offset
        );
    }

    void MetalCommandList::setConstantBuffer(
        uint32_t slot,
        RHIBufferHandle buffer,
        RHIShaderStage stage)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_setConstantBuffer(
            swiftCmdList,
            slot,
            handleToID(buffer),
            static_cast<::RHIShaderStage>(stage)
        );
    }

    void MetalCommandList::setTexture(
        uint32_t slot,
        RHITextureHandle texture,
        RHIShaderStage stage)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_setTexture(
            swiftCmdList,
            slot,
            handleToID(texture),
            static_cast<::RHIShaderStage>(stage)
        );
    }

    void MetalCommandList::setBuffer(
        uint32_t slot,
        RHIBufferHandle buffer,
        RHIShaderStage stage)
    {
        if (!swiftCmdList)
            return;
        MetalCommandList_setBuffer(
            swiftCmdList,
            slot,
            handleToID(buffer),
            static_cast<::RHIShaderStage>(stage)
        );
    }

    void MetalCommandList::setViewport(const RHIViewport& viewport)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_setViewport(
            swiftCmdList,
            &viewport
        );
    }

    void MetalCommandList::setViewports(
        const RHIViewport* viewports,
        uint32_t count)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_setViewports(
            swiftCmdList,
            viewports,
            count
        );
    }

    void MetalCommandList::setScissorRect(const RHIScissorRect& scissor)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_setScissorRect(swiftCmdList, &scissor);
    }

    void MetalCommandList::setScissorRects(
        const RHIScissorRect* scissors,
        uint32_t count)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_setScissorRects(
            swiftCmdList,
            scissors,
            count
        );
    }

    void MetalCommandList::draw(
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t startVertex,
        uint32_t startInstance)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_draw(
            swiftCmdList,
            vertexCount,
            instanceCount,
            startVertex,
            startInstance
        );
    }

    void MetalCommandList::drawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t startIndex,
        int32_t baseVertex,
        uint32_t startInstance)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_drawIndexed(
            swiftCmdList,
            indexCount,
            instanceCount,
            startIndex,
            baseVertex,
            startInstance
        );
    }

    void MetalCommandList::dispatch(
        uint32_t threadGroupCountX,
        uint32_t threadGroupCountY,
        uint32_t threadGroupCountZ)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_dispatch(
            swiftCmdList,
            threadGroupCountX,
            threadGroupCountY,
            threadGroupCountZ
        );
    }

    void MetalCommandList::transitionBarrier(
        RHITextureHandle texture,
        RHIResourceState before,
        RHIResourceState after)
    {
        // No-op for Metal (automatic resource transitions)
    }

    void MetalCommandList::transitionBarrier(
        RHIBufferHandle buffer,
        RHIResourceState before,
        RHIResourceState after)
    {
        // No-op for Metal (automatic resource transitions)
    }

    void MetalCommandList::copyBuffer(
        RHIBufferHandle src,
        RHIBufferHandle dst,
        size_t srcOffset,
        size_t dstOffset,
        size_t size)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_copyBuffer(
            swiftCmdList,
            handleToID(src),
            handleToID(dst),
            srcOffset,
            dstOffset,
            size
        );
    }

    void MetalCommandList::copyTexture(
        RHITextureHandle src,
        RHITextureHandle dst)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_copyTexture(
            swiftCmdList,
            handleToID(src),
            handleToID(dst)
        );
    }

    void MetalCommandList::copyBufferToTexture(
        RHIBufferHandle src,
        RHITextureHandle dst,
        uint32_t mipLevel,
        uint32_t arraySlice)
    {
        if(!swiftCmdList)
            return;
        MetalCommandList_copyBufferToTexture(
            swiftCmdList,
            handleToID(src),
            handleToID(dst),
            mipLevel,
            arraySlice
        );
    }

    void MetalCommandList::beginEvent(const char* name){
        if(!swiftCmdList)
            return;
        MetalCommandList_beginEvent(
            swiftCmdList,
            name
        );
    }

    void MetalCommandList::endEvent(){
        if(!swiftCmdList)
            return;
        MetalCommandList_endEvent(
            swiftCmdList
        );
    }

    void MetalCommandList::setMarker(const char* name){
        if(!swiftCmdList)
            return;
        MetalCommandList_setMarker(
            swiftCmdList,
            name
        );
    }

    void MetalCommandList::setDrawable(void* drawable)
    {
        if(!swiftCmdList) return;
        MetalCommandList_setDrawable(
            swiftCmdList,
            drawable
        );
    }
}

#endif // RHI_METAL
