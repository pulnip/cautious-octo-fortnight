#pragma once

#include "RHI/RHIPlatform.h"
#include "RHI/RHIDefinitions.h"
#include "Log/Log.hpp"

inline int hasFlag(RHIBufferUsageFlags flags, RHIBufferUsageFlags flag){
    return (flags & flag) == flag;
}

namespace RenderToy
{
    // RHI parameter validation helpers
    // Only enabled in debug builds (RHI_ENABLE_VALIDATION)
    namespace RHIValidation
    {
#if RHI_ENABLE_VALIDATION

        // Buffer validation
        inline bool validateBufferDesc(const struct RHIBufferCreateDesc& desc) {
            if (desc.size == 0) {
                LOG_ERROR(LOG_RHI, "Buffer size must be greater than 0");
                return false;
            }

            if (desc.usage == RHIBufferUsageFlags::None) {
                LOG_ERROR(LOG_RHI, "Buffer usage flags must be specified");
                return false;
            }

            if (hasFlag(desc.usage, RHIBufferUsageFlags::StructuredBuffer) && desc.stride == 0) {
                LOG_ERROR(LOG_RHI, "Structured buffer must have stride > 0");
                return false;
            }

            return true;
        }

        // Texture validation
        inline bool validateTextureDesc(const struct RHITextureCreateDesc& desc) {
            if (desc.width == 0 || desc.height == 0) {
                LOG_ERROR(LOG_RHI, "Texture dimensions must be greater than 0");
                return false;
            }

            if (desc.format == RHITextureFormat::Unknown) {
                LOG_ERROR(LOG_RHI, "Texture format must be specified");
                return false;
            }

            if (desc.usage == RHITextureUsageFlags::None) {
                LOG_ERROR(LOG_RHI, "Texture usage flags must be specified");
                return false;
            }

            if (desc.mipLevels == 0) {
                LOG_ERROR(LOG_RHI, "Texture must have at least 1 mip level");
                return false;
            }

            if (desc.arraySize == 0) {
                LOG_ERROR(LOG_RHI, "Texture array size must be at least 1");
                return false;
            }

            return true;
        }

        // Shader validation
        inline bool validateShaderDesc(const struct RHIShaderCreateDesc& desc) {
            if (desc.bytecode == nullptr || desc.bytecodeSize == 0) {
                LOG_ERROR(LOG_RHI, "Shader bytecode must be provided");
                return false;
            }

            return true;
        }

        // Pipeline state validation
        inline bool validateGraphicsPipelineDesc(const struct RHIGraphicsPipelineStateDesc& desc) {
            if (desc.vertexShader.index == 0) {
                LOG_ERROR(LOG_RHI, "Graphics pipeline must have vertex shader");
                return false;
            }

            if (desc.pixelShader.index == 0) {
                LOG_WARN(LOG_RHI, "Graphics pipeline has no pixel shader (depth-only pass?)");
            }

            if (desc.vertexLayout.elements == nullptr || desc.vertexLayout.elementCount == 0) {
                LOG_WARN(LOG_RHI, "Graphics pipeline has no vertex layout");
            }

            if (desc.renderTargetCount == 0 && desc.depthStencilFormat == RHITextureFormat::Unknown) {
                LOG_ERROR(LOG_RHI, "Graphics pipeline must have at least one render target or depth buffer");
                return false;
            }

            if (desc.renderTargetCount > RHI_MAX_RENDER_TARGETS) {
                LOG_ERROR(LOG_RHI, "Graphics pipeline has too many render targets (max {})",
                         RHI_MAX_RENDER_TARGETS);
                return false;
            }

            return true;
        }

        // Draw call validation
        inline bool validateDrawCall(uint32_t vertexCount, uint32_t instanceCount) {
            if (vertexCount == 0) {
                LOG_ERROR(LOG_RHI, "Draw call has 0 vertices");
                return false;
            }

            if (instanceCount == 0) {
                LOG_ERROR(LOG_RHI, "Draw call has 0 instances");
                return false;
            }

            return true;
        }

        inline bool validateDrawIndexedCall(uint32_t indexCount, uint32_t instanceCount) {
            if (indexCount == 0) {
                LOG_ERROR(LOG_RHI, "DrawIndexed call has 0 indices");
                return false;
            }

            if (instanceCount == 0) {
                LOG_ERROR(LOG_RHI, "DrawIndexed call has 0 instances");
                return false;
            }

            return true;
        }

#else
        // No-op in release builds
        inline constexpr bool validateBufferDesc(const struct RHIBufferCreateDesc&) { return true; }
        inline constexpr bool validateTextureDesc(const struct RHITextureCreateDesc&) { return true; }
        inline constexpr bool validateShaderDesc(const struct RHIShaderCreateDesc&) { return true; }
        inline constexpr bool validateGraphicsPipelineDesc(const struct RHIGraphicsPipelineStateDesc&) { return true; }
        inline constexpr bool validateDrawCall(uint32_t, uint32_t) { return true; }
        inline constexpr bool validateDrawIndexedCall(uint32_t, uint32_t) { return true; }
#endif
    }
}
