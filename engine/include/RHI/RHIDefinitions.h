#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "RHI/RHIPlatform.h"

#ifdef __cplusplus
extern "C"{
#endif

// Texture formats
typedef enum{
    Unknown = 0,

    // 8-bit formats
    R8_UNORM,
    R8_SNORM,
    R8_UINT,
    R8_SINT,

    // 16-bit formats
    R16_UNORM,
    R16_SNORM,
    R16_UINT,
    R16_SINT,
    R16_FLOAT,

    RG8_UNORM,
    RG8_SNORM,
    RG8_UINT,
    RG8_SINT,

    // 32-bit formats
    R32_UINT,
    R32_SINT,
    R32_FLOAT,

    RG16_UNORM,
    RG16_SNORM,
    RG16_UINT,
    RG16_SINT,
    RG16_FLOAT,

    RGBA8_UNORM,
    RGBA8_UNORM_SRGB,
    RGBA8_SNORM,
    RGBA8_UINT,
    RGBA8_SINT,

    BGRA8_UNORM,
    BGRA8_UNORM_SRGB,

    // 64-bit formats
    RG32_UINT,
    RG32_SINT,
    RG32_FLOAT,

    RGBA16_UNORM,
    RGBA16_SNORM,
    RGBA16_UINT,
    RGBA16_SINT,
    RGBA16_FLOAT,

    // 128-bit formats
    RGBA32_UINT,
    RGBA32_SINT,
    RGBA32_FLOAT,

    // Depth/stencil formats
    D16_UNORM,
    D24_UNORM_S8_UINT,
    D32_FLOAT,
    D32_FLOAT_S8_UINT,

    // Compressed formats
    BC1_UNORM,
    BC1_UNORM_SRGB,
    BC2_UNORM,
    BC2_UNORM_SRGB,
    BC3_UNORM,
    BC3_UNORM_SRGB,
    BC4_UNORM,
    BC4_SNORM,
    BC5_UNORM,
    BC5_SNORM,
    BC6H_UF16,
    BC6H_SF16,
    BC7_UNORM,
    BC7_UNORM_SRGB,
} RHITextureFormat;

// Buffer usage flags (can be combined)
typedef enum{
    BufNone             = 0,
    BufVertexBuffer     = 1 << 0,
    BufIndexBuffer      = 1 << 1,
    BufConstantBuffer   = 1 << 2,
    BufStructuredBuffer = 1 << 3,
    BufShaderResource   = 1 << 4,
    BufUnorderedAccess  = 1 << 5,
    BufIndirectArgs     = 1 << 6,
    BufCopySource       = 1 << 7,
    BufCopyDest         = 1 << 8,
    BufCPUWrite         = 1 << 9,
    BufTransferSrc      = 1 << 10
} RHIBufferUsageFlags;

// Texture usage flags (can be combined)
typedef enum{
    TexNone            = 0,
    TexShaderResource  = 1 << 0,
    TexRenderTarget    = 1 << 1,
    TexDepthStencil    = 1 << 2,
    TexUnorderedAccess = 1 << 3,
    TexCopySource      = 1 << 4,
    TexCopyDest        = 1 << 5,
} RHITextureUsageFlags;

// Resource states
typedef enum{
    Common,
    VertexBuffer,
    IndexBuffer,
    ConstantBuffer,
    ShaderResource,
    UnorderedAccess,
    RenderTarget,
    DepthStencilWrite,
    DepthStencilRead,
    CopySource,
    CopyDest,
    Present,
} RHIResourceState;

// Primitive topology
typedef enum{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
} RHIPrimitiveTopology;

// Shader stages
typedef enum{
    RHIShaderStage_Vertex,
    RHIShaderStage_Pixel,
    RHIShaderStage_Compute,
} RHIShaderStage;

// Comparison functions
typedef enum{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
} RHIComparisonFunc;

// Blend operations
typedef enum{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
} RHIBlendOp;

// Blend factors
typedef enum{
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
    SrcAlphaSat,
    BlendFactor,
    InvBlendFactor,
} RHIBlend;

// Cull modes
typedef enum{
    CullNone,
    Front,
    Back,
} RHICullMode;

// Fill modes
typedef enum{
    Solid,
    Wireframe,
} RHIFillMode;

// Filter modes
typedef enum{
    Point,
    Linear,
    Anisotropic,
} RHIFilter;

// Texture address modes
typedef enum{
    Wrap,
    Mirror,
    Clamp,
    Border,
} RHITextureAddressMode;

// Index buffer formats
typedef enum{
    UInt16,
    UInt32,
} RHIIndexFormat;

// Vertex input classification
typedef enum{
    PerVertex,
    PerInstance,
} RHIInputClassification;

// Load action for render targets
typedef enum{
    RHILoadAction_Load,           // Preserve existing contents
    RHILoadAction_Clear,          // Clear to specified color
    RHILoadAction_LoadDontCare,   // Don't care about existing contents
} RHILoadAction;

// Store action for render targets
typedef enum{
    RHIStoreAction_Store,           // Save contents
    RHIStoreAction_DontCare,   // Don't care about storing
} RHIStoreAction;

// Clear color value
typedef struct{
    float r, g, b, a;
} RHIClearColor;

// Clear depth/stencil value
typedef struct{
    float depth;
    uint8_t stencil;
} RHIClearDepthStencil;

// Viewport
typedef struct{
    float x, y;
    float width, height;
    float minDepth, maxDepth;
} RHIViewport;

// Scissor rectangle
typedef struct{
    int32_t left, top;
    int32_t right, bottom;
} RHIScissorRect;

// Rasterizer state
typedef struct{
    RHIFillMode fillMode;
    RHICullMode cullMode;
    bool frontCounterClockwise;
    int32_t depthBias;
    float depthBiasClamp;
    float slopeScaledDepthBias;
    bool depthClipEnable;
    bool multisampleEnable;
    bool antialiasedLineEnable;
} RHIRasterizerState;

// Depth/stencil state
typedef struct{
    bool depthEnable;
    bool depthWriteEnable;
    RHIComparisonFunc depthFunc;
    bool stencilEnable;
    uint8_t stencilReadMask;
    uint8_t stencilWriteMask;
} RHIDepthStencilState;

// Blend state for a single render target
typedef struct{
    bool blendEnable;
    RHIBlend srcBlend;
    RHIBlend dstBlend;
    RHIBlendOp blendOp;
    RHIBlend srcBlendAlpha;
    RHIBlend dstBlendAlpha;
    RHIBlendOp blendOpAlpha;
    uint8_t renderTargetWriteMask; // RGBA
} RHIRenderTargetBlendState;

// Blend state
typedef struct{
    bool alphaToCoverageEnable;
    bool independentBlendEnable;
    RHIRenderTargetBlendState renderTargets[8];
} RHIBlendState;

#ifdef __cplusplus
}
#endif

// Maximum constants
#define RHI_MAX_RENDER_TARGETS  (8)
#define RHI_MAX_VERTEX_BUFFERS  (16)
#define RHI_MAX_VERTEX_ELEMENTS (32)
#define RHI_FRAMES_IN_FLIGHT    (3)
