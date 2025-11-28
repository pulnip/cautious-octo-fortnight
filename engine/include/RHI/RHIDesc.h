#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

// Device creation descriptor
typedef struct{
    bool enableDebugLayer;
    bool enableGPUValidation;
    bool enableValidation;  // Shorthand for debug + validation
    const char* applicationName;
    void* windowHandle;  // Optional window handle for device creation
} RHIDeviceCreateDesc;

// Buffer creation descriptor
typedef struct{
    size_t size;
    RHIBufferUsageFlags usage;
    uint32_t stride; // For structured buffers
    const void* initialData;
    const char* debugName;
} RHIBufferCreateDesc;

// Texture creation descriptor
typedef struct{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t arraySize;
    RHITextureFormat format;
    RHITextureUsageFlags usage;
    RHIResourceState initialState;
    RHIClearColor clearColor;
    RHIClearDepthStencil clearDepthStencil;
    const char* debugName;
} RHITextureCreateDesc;

// Shader creation descriptor
typedef struct{
    RHIShaderStage stage;
    const void* bytecode;
    size_t bytecodeSize;
    const char* entryPoint; // Optional, platform-specific
    const char* debugName;
} RHIShaderCreateDesc;

// Vertex element descriptor (input layout)
typedef struct{
    const char* semanticName;
    uint32_t semanticIndex;
    RHITextureFormat format;
    uint32_t inputSlot;
    uint32_t alignedByteOffset;
    RHIInputClassification classification;
    uint32_t instanceDataStepRate; // For per-instance data
} RHIVertexElement;

// Vertex layout descriptor
typedef struct{
    const RHIVertexElement* elements;
    uint32_t elementCount;
} RHIVertexLayout;

// Graphics pipeline state descriptor
typedef struct{
    uint64_t vertexShaderIndex;
    uint64_t pixelShaderIndex;

    RHIVertexLayout vertexLayout;
    RHIPrimitiveTopology topology;

    RHIRasterizerState rasterizer;
    RHIDepthStencilState depthStencil;
    RHIBlendState blend;

    RHITextureFormat renderTargetFormats[RHI_MAX_RENDER_TARGETS];
    uint32_t renderTargetCount;
    RHITextureFormat depthStencilFormat;
    const char* debugName;
} RHIGraphicsPipelineStateDesc;

// Compute pipeline state descriptor
typedef struct{
    uint64_t computeShader;
    const char* debugName;
} RHIComputePipelineStateDesc;

// Swapchain creation descriptor
typedef struct{
    void* windowHandle;  // Platform-specific window handle
    uint32_t width;
    uint32_t height;
    RHITextureFormat format;
    uint32_t bufferCount; // Triple buffering
    bool vsync; // VSync enabled by default
    bool allowTearing; // Variable refresh rate
    const char* debugName;
} RHISwapchainCreateDesc;

#ifdef __cplusplus
}
#endif