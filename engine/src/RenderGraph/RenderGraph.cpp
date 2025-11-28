#include "RenderGraph/RenderGraph.hpp"
#include "RenderGraph/RenderPass.hpp"
#include "RenderGraph/RenderGraphBuilder.hpp"
#include "RenderGraph/RenderGraphResources.hpp"
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <queue>

namespace RenderToy {

RenderGraph::RenderGraph(RHIDevice* device)
    : m_device(device)
{
    if (!m_device) {
        throw std::runtime_error("RenderGraph: device cannot be null");
    }
}

RenderGraph::~RenderGraph() {
    // Destroy all transient resources
    for (RGTextureHandle handle : m_allTextureHandles) {
        const RGTexture& texture = m_textures[handle];
        if (!texture.isImported && texture.rhiHandle != RHI_INVALID_TEXTURE_HANDLE) {
            m_device->destroyTexture(texture.rhiHandle);
        }
    }

    for (RGBufferHandle handle : m_allBufferHandles) {
        const RGBuffer& buffer = m_buffers[handle];
        if (!buffer.isImported && buffer.rhiHandle != RHI_INVALID_BUFFER_HANDLE) {
            m_device->destroyBuffer(buffer.rhiHandle);
        }
    }
}

// ========== Resource Management ==========

RGTextureHandle RenderGraph::createTexture(const std::string& name, const RHITextureCreateDesc& desc) {
    RGTexture texture;
    texture.name = name;
    texture.desc = desc;
    texture.isImported = false;

    RGTextureHandle handle = m_textures.push(std::move(texture));
    m_textureNameMap[name] = handle;
    m_allTextureHandles.push_back(handle);

    return handle;
}

RGBufferHandle RenderGraph::createBuffer(const std::string& name, const RHIBufferCreateDesc& desc) {
    RGBuffer buffer;
    buffer.name = name;
    buffer.desc = desc;
    buffer.isImported = false;

    RGBufferHandle handle = m_buffers.push(std::move(buffer));
    m_bufferNameMap[name] = handle;
    m_allBufferHandles.push_back(handle);

    return handle;
}

RGTextureHandle RenderGraph::importTexture(const std::string& name, RHITextureHandle handle) {
    RGTexture texture;
    texture.name = name;
    texture.rhiHandle = handle;
    texture.isImported = true;

    RGTextureHandle rgHandle = m_textures.push(std::move(texture));
    m_textureNameMap[name] = rgHandle;
    m_allTextureHandles.push_back(rgHandle);

    return rgHandle;
}

RGBufferHandle RenderGraph::importBuffer(const std::string& name, RHIBufferHandle handle) {
    RGBuffer buffer;
    buffer.name = name;
    buffer.rhiHandle = handle;
    buffer.isImported = true;

    RGBufferHandle rgHandle = m_buffers.push(std::move(buffer));
    m_bufferNameMap[name] = rgHandle;
    m_allBufferHandles.push_back(rgHandle);

    return rgHandle;
}

void RenderGraph::registerTextureRead(RGTextureHandle handle, uint32_t passIndex) {
    RGTexture& texture = m_textures[handle];
    texture.firstUsedPass = std::min(texture.firstUsedPass, passIndex);
    texture.lastUsedPass = std::max(texture.lastUsedPass, passIndex);
}

void RenderGraph::registerTextureWrite(RGTextureHandle handle, uint32_t passIndex) {
    RGTexture& texture = m_textures[handle];
    texture.firstUsedPass = std::min(texture.firstUsedPass, passIndex);
    texture.lastUsedPass = std::max(texture.lastUsedPass, passIndex);
}

void RenderGraph::registerBufferRead(RGBufferHandle handle, uint32_t passIndex) {
    RGBuffer& buffer = m_buffers[handle];
    buffer.firstUsedPass = std::min(buffer.firstUsedPass, passIndex);
    buffer.lastUsedPass = std::max(buffer.lastUsedPass, passIndex);
}

void RenderGraph::registerBufferWrite(RGBufferHandle handle, uint32_t passIndex) {
    RGBuffer& buffer = m_buffers[handle];
    buffer.firstUsedPass = std::min(buffer.firstUsedPass, passIndex);
    buffer.lastUsedPass = std::max(buffer.lastUsedPass, passIndex);
}

RHITextureHandle RenderGraph::getRHITexture(RGTextureHandle handle) const {
    const RGTexture& texture = m_textures[handle];
    return texture.rhiHandle;
}

RHITextureHandle RenderGraph::getRHITexture(const std::string& name) const {
    auto it = m_textureNameMap.find(name);
    if (it != m_textureNameMap.end()) {
        return getRHITexture(it->second);
    }
    return RHI_INVALID_TEXTURE_HANDLE;
}

RHIBufferHandle RenderGraph::getRHIBuffer(RGBufferHandle handle) const {
    const RGBuffer& buffer = m_buffers[handle];
    return buffer.rhiHandle;
}

RHIBufferHandle RenderGraph::getRHIBuffer(const std::string& name) const {
    auto it = m_bufferNameMap.find(name);
    if (it != m_bufferNameMap.end()) {
        return getRHIBuffer(it->second);
    }
    return RHI_INVALID_BUFFER_HANDLE;
}

// ========== Compilation ==========

void RenderGraph::compile() {
    m_isCompiled = false;

    // Stage 1: Run setup functions to declare resources
    runSetupFunctions();

    // Stage 2: Build dependency graph
    buildDependencyGraph();

    // Stage 3: Topological sort
    topologicalSort();

    // Stage 4: Cull unused passes (optional, for now skip)
    // cullUnusedPasses();

    // Stage 5: Allocate physical resources
    allocateResources();

    m_isCompiled = true;
}

void RenderGraph::runSetupFunctions() {
    // Setup functions were already run during addPass()
    // Here we just register resource lifetimes based on dependencies

    for (size_t i = 0; i < m_passes.size(); ++i) {
        RenderPass* pass = m_passes[i].get();

        // Register resource usage with lifetime tracking
        const auto& deps = pass->getDependencies();
        for (const auto& dep : deps) {
            if (dep.isTexture()) {
                if (dep.accessMode == ResourceAccessMode::Read ||
                    dep.accessMode == ResourceAccessMode::ReadWrite) {
                    registerTextureRead(dep.textureHandle, static_cast<uint32_t>(i));
                }
                if (dep.accessMode == ResourceAccessMode::Write ||
                    dep.accessMode == ResourceAccessMode::ReadWrite) {
                    registerTextureWrite(dep.textureHandle, static_cast<uint32_t>(i));
                }
            } else if (dep.isBuffer()) {
                if (dep.accessMode == ResourceAccessMode::Read ||
                    dep.accessMode == ResourceAccessMode::ReadWrite) {
                    registerBufferRead(dep.bufferHandle, static_cast<uint32_t>(i));
                }
                if (dep.accessMode == ResourceAccessMode::Write ||
                    dep.accessMode == ResourceAccessMode::ReadWrite) {
                    registerBufferWrite(dep.bufferHandle, static_cast<uint32_t>(i));
                }
            }
        }
    }
}

void RenderGraph::buildDependencyGraph() {
    const size_t passCount = m_passes.size();
    m_passEdges.clear();
    m_passEdges.resize(passCount);

    // For each pass, determine which later passes depend on it
    // Pass B depends on Pass A if:
    // - A writes to a resource that B reads
    // - A writes to a resource that B writes (WAW dependency)

    for (size_t i = 0; i < passCount; ++i) {
        const RenderPass* passA = m_passes[i].get();
        auto textureWrites = passA->getTextureWrites();
        auto bufferWrites = passA->getBufferWrites();

        // Check all later passes
        for (size_t j = i + 1; j < passCount; ++j) {
            const RenderPass* passB = m_passes[j].get();
            auto textureReads = passB->getTextureReads();
            auto textureWritesB = passB->getTextureWrites();
            auto bufferReads = passB->getBufferReads();
            auto bufferWritesB = passB->getBufferWrites();

            bool hasDependency = false;

            // Check texture dependencies
            for (auto writeHandle : textureWrites) {
                // RAW (Read After Write)
                if (std::find(textureReads.begin(), textureReads.end(), writeHandle) != textureReads.end()) {
                    hasDependency = true;
                    break;
                }
                // WAW (Write After Write)
                if (std::find(textureWritesB.begin(), textureWritesB.end(), writeHandle) != textureWritesB.end()) {
                    hasDependency = true;
                    break;
                }
            }

            // Check buffer dependencies
            if (!hasDependency) {
                for (auto writeHandle : bufferWrites) {
                    // RAW
                    if (std::find(bufferReads.begin(), bufferReads.end(), writeHandle) != bufferReads.end()) {
                        hasDependency = true;
                        break;
                    }
                    // WAW
                    if (std::find(bufferWritesB.begin(), bufferWritesB.end(), writeHandle) != bufferWritesB.end()) {
                        hasDependency = true;
                        break;
                    }
                }
            }

            if (hasDependency) {
                // Pass j depends on pass i
                m_passEdges[i].push_back(static_cast<uint32_t>(j));
            }
        }
    }
}

void RenderGraph::topologicalSort() {
    const size_t passCount = m_passes.size();

    // Calculate in-degrees
    std::vector<uint32_t> inDegree(passCount, 0);
    for (size_t i = 0; i < passCount; ++i) {
        for (uint32_t neighbor : m_passEdges[i]) {
            inDegree[neighbor]++;
        }
    }

    // Kahn's algorithm for topological sort
    std::queue<uint32_t> queue;
    for (uint32_t i = 0; i < passCount; ++i) {
        if (inDegree[i] == 0) {
            queue.push(i);
        }
    }

    m_sortedPassIndices.clear();
    while (!queue.empty()) {
        uint32_t current = queue.front();
        queue.pop();
        m_sortedPassIndices.push_back(current);

        for (uint32_t neighbor : m_passEdges[current]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                queue.push(neighbor);
            }
        }
    }

    // Check for cycles
    if (m_sortedPassIndices.size() != passCount) {
        throw std::runtime_error("RenderGraph: Cyclic dependency detected!");
    }
}

void RenderGraph::cullUnusedPasses() {
    // TODO: Implement pass culling
    // For now, we execute all passes
}

void RenderGraph::allocateResources() {
    // Allocate physical RHI resources for all transient resources

    for (RGTextureHandle handle : m_allTextureHandles) {
        RGTexture& texture = m_textures[handle];
        if (!texture.isImported) {
            // Create the actual RHI texture
            texture.rhiHandle = m_device->createTexture(texture.desc);
        }
    }

    for (RGBufferHandle handle : m_allBufferHandles) {
        RGBuffer& buffer = m_buffers[handle];
        if (!buffer.isImported) {
            // Create the actual RHI buffer
            buffer.rhiHandle = m_device->createBuffer(buffer.desc);
        }
    }
}

// ========== Execution ==========

void RenderGraph::execute() {
    if (!m_isCompiled) {
        throw std::runtime_error("RenderGraph: Must call compile() before execute()");
    }

    // Create a command list for the entire graph
    // TODO: In the future, we might want to split this into multiple command lists
    RHICommandList* cmdList = m_device->beginCommandList();

    // Create resource accessor
    RenderGraphResources resources(this);

    // Execute passes in sorted order
    for (uint32_t passIndex : m_sortedPassIndices) {
        const RenderPass* pass = m_passes[passIndex].get();

        // TODO: Insert resource barriers/transitions here based on dependencies

        // Execute the pass
        pass->getExecuteFunc()(resources, *cmdList);
    }

    // Submit the command list
    m_device->submitCommandList(cmdList);
}

} // namespace RenderToy
