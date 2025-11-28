#pragma once

#include <string>
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHIDesc.h"
#include "RHI/RHITexture.hpp"
#include "RenderGraph/RGHandle.hpp"

namespace RenderToy {
    // Forward declarations
    class RenderGraph;
    class RenderPass;

    /// @brief Builder class for declaring RenderGraph resources in pass setup
    /// @details Used in the setup lambda to create resources and declare dependencies
    class RenderGraphBuilder {
    public:
        explicit RenderGraphBuilder(RenderPass* pass);

        /// @brief Create a new transient texture resource
        /// @param name Debug name for the texture
        /// @param desc Texture creation descriptor
        /// @return Handle to the created texture
        RGTextureHandle createTexture(const std::string& name, const RHITextureCreateDesc& desc);

        /// @brief Create a new transient buffer resource
        /// @param name Debug name for the buffer
        /// @param desc Buffer creation descriptor
        /// @return Handle to the created buffer
        RGBufferHandle createBuffer(const std::string& name, const RHIBufferCreateDesc& desc);

        /// @brief Import an external texture into the graph
        /// @param name Name to reference this texture
        /// @param handle External RHI texture handle
        /// @return Handle to use within the graph
        RGTextureHandle importTexture(const std::string& name, RHITextureHandle handle);

        /// @brief Import an external buffer into the graph
        /// @param name Name to reference this buffer
        /// @param handle External RHI buffer handle
        /// @return Handle to use within the graph
        RGBufferHandle importBuffer(const std::string& name, RHIBufferHandle handle);

        /// @brief Declare that this pass reads from a texture
        /// @param handle Texture to read from
        void readTexture(RGTextureHandle handle);

        /// @brief Declare that this pass writes to a texture
        /// @param handle Texture to write to
        void writeTexture(RGTextureHandle handle);

        /// @brief Declare that this pass reads from a buffer
        /// @param handle Buffer to read from
        void readBuffer(RGBufferHandle handle);

        /// @brief Declare that this pass writes to a buffer
        /// @param handle Buffer to write to
        void writeBuffer(RGBufferHandle handle);

    private:
        RenderPass* m_pass = nullptr;
        RenderGraph* m_graph = nullptr;

        friend class RenderGraph;
    };
} // namespace RenderToy
