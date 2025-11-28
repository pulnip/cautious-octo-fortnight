/*
 * MetalCommandList C Bridge Header
 *
 * C-compatible interfaces for Swift implementation of Metal command lists.
 */

#pragma once

#include "RHI/RHIDesc.h"
#include "MetalTypes.h"


#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// MetalCommandList C Bridge Functions
// ============================================================================

// Command list lifecycle
void MetalCommandList_destroy(
    MetalCommandListPtr cmdList
);
void MetalCommandList_begin(
    MetalCommandListPtr cmdList
);
void MetalCommandList_close(
    MetalCommandListPtr cmdList
);
void MetalCommandList_reset(
    MetalCommandListPtr cmdList
);

// Render pass control (using uint64_t for handle IDs)
void MetalCommandList_beginRenderPass(
    MetalCommandListPtr cmdList,
    uint64_t renderTarget,
    uint64_t depthStencil,
    RHILoadAction loadAction,
    RHIStoreAction storeAction,
    const RHIClearColor* clearColor
);

void MetalCommandList_beginRenderPassMultiTarget(
    MetalCommandListPtr cmdList,
    const uint64_t* renderTargets,
    uint32_t renderTargetCount,
    uint64_t depthStencil,
    RHILoadAction loadAction,
    RHIStoreAction storeAction,
    const RHIClearColor* clearColor
);

void MetalCommandList_endRenderPass(
    MetalCommandListPtr cmdList
);

// Pipeline state
void MetalCommandList_setPipelineState(
    MetalCommandListPtr cmdList,
    MetalPipelineStateID pso
);

// Vertex and index buffers
void MetalCommandList_setVertexBuffer(
    MetalCommandListPtr cmdList,
    uint32_t slot,
    MetalBufferID bufferID,
    uint32_t stride,
    uint32_t offset
);

void MetalCommandList_setIndexBuffer(
    MetalCommandListPtr cmdList,
    MetalBufferID bufferID,
    RHIIndexFormat format,
    uint32_t offset
);

// Constant buffers
void MetalCommandList_setConstantBuffer(
    MetalCommandListPtr cmdList,
    uint32_t slot,
    MetalBufferID bufferID,
    RHIShaderStage stage
);

// Shader resources
void MetalCommandList_setTexture(
    MetalCommandListPtr cmdList,
    uint32_t slot,
    MetalTextureID texture,
    RHIShaderStage stage
);

void MetalCommandList_setBuffer(
    MetalCommandListPtr cmdList,
    uint32_t slot,
    MetalBufferID buffer,
    RHIShaderStage stage
);

// Viewport and scissor
void MetalCommandList_setViewport(
    MetalCommandListPtr cmdList,
    const RHIViewport* viewport
);

void MetalCommandList_setViewports(
    MetalCommandListPtr cmdList,
    const RHIViewport* viewports,
    uint32_t count
);

void MetalCommandList_setScissorRect(
    MetalCommandListPtr cmdList,
    const RHIScissorRect* scissor
);

void MetalCommandList_setScissorRects(
    MetalCommandListPtr cmdList,
    const RHIScissorRect* scissors,
    uint32_t count
);

// Draw commands
void MetalCommandList_draw(
    MetalCommandListPtr cmdList,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t startVertex,
    uint32_t startInstance
);

void MetalCommandList_drawIndexed(
    MetalCommandListPtr cmdList,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t startIndex,
    int32_t baseVertex,
    uint32_t startInstance
);

// Compute dispatch
void MetalCommandList_dispatch(
    MetalCommandListPtr cmdList,
    uint32_t threadGroupCountX,
    uint32_t threadGroupCountY,
    uint32_t threadGroupCountZ
);

// Copy operations
void MetalCommandList_copyBuffer(
    MetalCommandListPtr cmdList,
    MetalBufferID src,
    MetalBufferID dst,
    size_t srcOffset,
    size_t dstOffset,
    size_t size
);

void MetalCommandList_copyTexture(
    MetalCommandListPtr cmdList,
    MetalTextureID src,
    MetalTextureID dst
);

void MetalCommandList_copyBufferToTexture(
    MetalCommandListPtr cmdList,
    MetalBufferID src,
    MetalTextureID dst,
    uint32_t mipLevel,
    uint32_t arraySlice
);

// Debug markers
void MetalCommandList_beginEvent(
    MetalCommandListPtr cmdList,
    const char* name
);
void MetalCommandList_endEvent(
    MetalCommandListPtr cmdList
);
void MetalCommandList_setMarker(
    MetalCommandListPtr cmdList,
    const char* name
);

// Metal-specific: Set the drawable for rendering
void MetalCommandList_setDrawable(
    MetalCommandListPtr cmdList,
    void* drawable
);

#ifdef __cplusplus
}
#endif
