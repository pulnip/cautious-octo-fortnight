#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "slot_map.hpp"
#include "RHI/RHIDevice.hpp"
#include "RHI/RHITexture.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHIDesc.h"
#include "RenderGraph//RGHandle.hpp"
#include "RenderGraph/RenderPass.hpp"
#include "RenderGraph/RenderGraphBuilder.hpp"
#include "RenderGraph/RenderGraphResources.hpp"

namespace RenderToy {
    /// @brief RenderGraph texture resource
    struct RGTexture {
        std::string name;
        RHITextureCreateDesc desc;
        RHITextureHandle rhiHandle = RHI_INVALID_TEXTURE_HANDLE;
        bool isImported = false; // External resource

        // Lifetime tracking
        uint32_t firstUsedPass = UINT32_MAX;
        uint32_t lastUsedPass = 0;
    };

    /// @brief RenderGraph buffer resource
    struct RGBuffer {
        std::string name;
        RHIBufferCreateDesc desc;
        RHIBufferHandle rhiHandle = RHI_INVALID_BUFFER_HANDLE;
        bool isImported = false; // External resource

        // Lifetime tracking
        uint32_t firstUsedPass = UINT32_MAX;
        uint32_t lastUsedPass = 0;
    };

    /// @brief Main RenderGraph class (Option A: Builder + Compile)
    /// @details Manages render passes, resources, and dependencies
    class RenderGraph {
    public:
        explicit RenderGraph(RHIDevice* device);
        ~RenderGraph();

        // ========== Pass Management ==========

        /// @brief Add a render pass to the graph
        /// @tparam ReturnType Type returned by setup function
        /// @param name Pass name for debugging
        /// @param setupFunc Setup lambda: declare resources and dependencies
        /// @param executeFunc Execute lambda: record rendering commands
        /// @return Value returned by setupFunc (typically resource handles)
        template<typename ReturnType = void>
        ReturnType addPass(
            const std::string& name,
            std::function<ReturnType(RenderGraphBuilder&)> setupFunc,
            PassExecuteFunc executeFunc
        );

        // ========== Graph Compilation ==========

        /// @brief Compile the graph: analyze dependencies, sort passes, allocate resources
        /// @details Must be called after all passes are added, before execute()
        void compile();

        // ========== Graph Execution ==========

        /// @brief Execute the compiled graph: submit commands to GPU
        /// @details Executes passes in dependency order
        void execute();

        // ========== Resource Management ==========

        /// @brief Get RHI texture handle from RenderGraph handle
        RHITextureHandle getRHITexture(RGTextureHandle handle) const;

        /// @brief Get RHI texture handle by name
        RHITextureHandle getRHITexture(const std::string& name) const;

        /// @brief Get RHI buffer handle from RenderGraph handle
        RHIBufferHandle getRHIBuffer(RGBufferHandle handle) const;

        /// @brief Get RHI buffer handle by name
        RHIBufferHandle getRHIBuffer(const std::string& name) const;

        /// @brief Get device
        RHIDevice* getDevice() const { return m_device; }

    private:
        // ========== Internal Resource Management ==========

        RGTextureHandle createTexture(const std::string& name, const RHITextureCreateDesc& desc);
        RGBufferHandle createBuffer(const std::string& name, const RHIBufferCreateDesc& desc);
        RGTextureHandle importTexture(const std::string& name, RHITextureHandle handle);
        RGBufferHandle importBuffer(const std::string& name, RHIBufferHandle handle);

        void registerTextureRead(RGTextureHandle handle, uint32_t passIndex);
        void registerTextureWrite(RGTextureHandle handle, uint32_t passIndex);
        void registerBufferRead(RGBufferHandle handle, uint32_t passIndex);
        void registerBufferWrite(RGBufferHandle handle, uint32_t passIndex);

        // ========== Compilation Stages ==========

        /// @brief Run setup functions to declare resources
        void runSetupFunctions();

        /// @brief Build dependency graph between passes
        void buildDependencyGraph();

        /// @brief Topologically sort passes by dependencies
        void topologicalSort();

        /// @brief Cull unused passes
        void cullUnusedPasses();

        /// @brief Allocate physical RHI resources
        void allocateResources();

        // ========== Data Members ==========

        RHIDevice* m_device = nullptr;

        // Passes
        std::vector<std::unique_ptr<RenderPass>> m_passes;
        std::vector<uint32_t> m_sortedPassIndices; // Execution order after compile

        // Resources
        slot_map<RGTexture> m_textures;
        slot_map<RGBuffer> m_buffers;
        std::vector<RGTextureHandle> m_allTextureHandles; // Track all texture handles for iteration
        std::vector<RGBufferHandle> m_allBufferHandles;   // Track all buffer handles for iteration

        // Name lookup
        std::unordered_map<std::string, RGTextureHandle> m_textureNameMap;
        std::unordered_map<std::string, RGBufferHandle> m_bufferNameMap;

        // Dependency graph (adjacency list)
        std::vector<std::vector<uint32_t>> m_passEdges; // m_passEdges[i] = passes that depend on pass i

        // State
        bool m_isCompiled = false;

        friend class RenderGraphBuilder;
        friend class RenderGraphResources;
    };

    // ========== Template Implementation ==========

    template<typename ReturnType>
    ReturnType RenderGraph::addPass(
        const std::string& name,
        std::function<ReturnType(RenderGraphBuilder&)> setupFunc,
        PassExecuteFunc executeFunc
    ) {
        // Wrap the typed setup function into a type-erased version
        PassSetupFunc wrappedSetup = [setupFunc](RenderGraphBuilder& builder) -> std::any {
            if constexpr (std::is_void_v<ReturnType>) {
                setupFunc(builder);
                return std::any();
            } else {
                return std::any(setupFunc(builder));
            }
        };

        // Create the pass
        auto pass = std::make_unique<RenderPass>(name, wrappedSetup, executeFunc);
        RenderPass* passPtr = pass.get();
        m_passes.push_back(std::move(pass));

        // Run setup immediately so we can return resource handles
        RenderGraphBuilder builder(passPtr);
        builder.m_graph = this;

        if constexpr (std::is_void_v<ReturnType>) {
            setupFunc(builder);
        } else {
            ReturnType result = setupFunc(builder);
            passPtr->setReturnValue(std::any(result));
            return result;
        }
    }
} // namespace RenderToy
