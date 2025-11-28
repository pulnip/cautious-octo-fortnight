#include "RenderGraph/RenderGraphBuilder.hpp"
#include "RenderGraph/RenderPass.hpp"
#include "RenderGraph/RenderGraph.hpp"

namespace RenderToy {

RenderGraphBuilder::RenderGraphBuilder(RenderPass* pass)
    : m_pass(pass)
    , m_graph(nullptr)
{
}

RGTextureHandle RenderGraphBuilder::createTexture(const std::string& name, const RHITextureCreateDesc& desc) {
    if (!m_graph) {
        return RG_INVALID_TEXTURE;
    }
    return m_graph->createTexture(name, desc);
}

RGBufferHandle RenderGraphBuilder::createBuffer(const std::string& name, const RHIBufferCreateDesc& desc) {
    if (!m_graph) {
        return RG_INVALID_BUFFER;
    }
    return m_graph->createBuffer(name, desc);
}

RGTextureHandle RenderGraphBuilder::importTexture(const std::string& name, RHITextureHandle handle) {
    if (!m_graph) {
        return RG_INVALID_TEXTURE;
    }
    return m_graph->importTexture(name, handle);
}

RGBufferHandle RenderGraphBuilder::importBuffer(const std::string& name, RHIBufferHandle handle) {
    if (!m_graph) {
        return RG_INVALID_BUFFER;
    }
    return m_graph->importBuffer(name, handle);
}

void RenderGraphBuilder::readTexture(RGTextureHandle handle) {
    ResourceDependency dep;
    dep.textureHandle = handle;
    dep.accessMode = ResourceAccessMode::Read;
    m_pass->addDependency(dep);
}

void RenderGraphBuilder::writeTexture(RGTextureHandle handle) {
    ResourceDependency dep;
    dep.textureHandle = handle;
    dep.accessMode = ResourceAccessMode::Write;
    m_pass->addDependency(dep);
}

void RenderGraphBuilder::readBuffer(RGBufferHandle handle) {
    ResourceDependency dep;
    dep.bufferHandle = handle;
    dep.accessMode = ResourceAccessMode::Read;
    m_pass->addDependency(dep);
}

void RenderGraphBuilder::writeBuffer(RGBufferHandle handle) {
    ResourceDependency dep;
    dep.bufferHandle = handle;
    dep.accessMode = ResourceAccessMode::Write;
    m_pass->addDependency(dep);
}

} // namespace RenderToy
