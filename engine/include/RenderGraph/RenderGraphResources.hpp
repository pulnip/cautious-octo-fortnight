#pragma once

#include <string>
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHITexture.hpp"
#include "RenderGraph/RGHandle.hpp"

namespace RenderToy
{
    // Forward declarations
    class RenderGraph;

    /// @brief Resource accessor for execute lambda
    /// @details Provides access to actual RHI resources during pass execution
    class RenderGraphResources {
    public:
        explicit RenderGraphResources(RenderGraph* graph);

        /// @brief Get the actual RHI texture handle for a render graph texture
        /// @param handle RenderGraph texture handle
        /// @return Corresponding RHI texture handle
        RHITextureHandle getTexture(RGTextureHandle handle) const;

        /// @brief Get the actual RHI texture handle by name
        /// @param name Resource name
        /// @return Corresponding RHI texture handle
        RHITextureHandle getTexture(const std::string& name) const;

        /// @brief Get the actual RHI buffer handle for a render graph buffer
        /// @param handle RenderGraph buffer handle
        /// @return Corresponding RHI buffer handle
        RHIBufferHandle getBuffer(RGBufferHandle handle) const;

        /// @brief Get the actual RHI buffer handle by name
        /// @param name Resource name
        /// @return Corresponding RHI buffer handle
        RHIBufferHandle getBuffer(const std::string& name) const;

    private:
        RenderGraph* m_graph = nullptr;

        friend class RenderGraph;
    };
} // namespace RenderToy
