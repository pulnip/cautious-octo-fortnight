#pragma once

#include <any>
#include <functional>
#include <string>
#include <vector>
#include "RHI/RHICommandList.hpp"
#include "RenderGraph/RGHandle.hpp"
#include "RenderGraph/RenderGraphBuilder.hpp"
#include "RenderGraph/RenderGraphResources.hpp"

namespace RenderToy
{
    /// @brief Function signature for pass setup lambda
    /// @details Called during graph construction to declare resources and dependencies
    using PassSetupFunc = std::function<std::any(RenderGraphBuilder&)>;

    /// @brief Function signature for pass execute lambda
    /// @details Called during graph execution to record rendering commands
    using PassExecuteFunc = std::function<void(const RenderGraphResources&, RHICommandList&)>;

    /// @brief Resource access mode
    enum class ResourceAccessMode {
        Read,
        Write,
        ReadWrite
    };

    /// @brief Resource dependency information
    struct ResourceDependency {
        RGTextureHandle textureHandle = RG_INVALID_TEXTURE;
        RGBufferHandle bufferHandle = RG_INVALID_BUFFER;
        ResourceAccessMode accessMode = ResourceAccessMode::Read;

        bool isTexture() const { return textureHandle != RG_INVALID_TEXTURE; }
        bool isBuffer() const { return bufferHandle != RG_INVALID_BUFFER; }
    };

    /// @brief Internal representation of a render pass
    class RenderPass {
    public:
        RenderPass(std::string name, PassSetupFunc setupFunc, PassExecuteFunc executeFunc);

        /// @brief Get the pass name
        const std::string& getName() const { return m_name; }

        /// @brief Get resource dependencies (reads + writes)
        const std::vector<ResourceDependency>& getDependencies() const { return m_dependencies; }

        /// @brief Get the setup function
        const PassSetupFunc& getSetupFunc() const { return m_setupFunc; }

        /// @brief Get the execute function
        const PassExecuteFunc& getExecuteFunc() const { return m_executeFunc; }

        /// @brief Get the return value from setup function
        const std::any& getReturnValue() const { return m_returnValue; }

        /// @brief Set the return value from setup function
        void setReturnValue(std::any value) { m_returnValue = std::move(value); }

        /// @brief Add a resource dependency
        void addDependency(ResourceDependency dep);

        /// @brief Get all texture reads
        std::vector<RGTextureHandle> getTextureReads() const;

        /// @brief Get all texture writes
        std::vector<RGTextureHandle> getTextureWrites() const;

        /// @brief Get all buffer reads
        std::vector<RGBufferHandle> getBufferReads() const;

        /// @brief Get all buffer writes
        std::vector<RGBufferHandle> getBufferWrites() const;

    private:
        std::string m_name;
        PassSetupFunc m_setupFunc;
        PassExecuteFunc m_executeFunc;
        std::vector<ResourceDependency> m_dependencies;
        std::any m_returnValue; // Return value from setup function

        friend class RenderGraphBuilder;
        friend class RenderGraph;
    };
} // namespace RenderToy
