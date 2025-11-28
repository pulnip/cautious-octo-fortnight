#include "RenderGraph/RenderGraphResources.hpp"
#include "RenderGraph/RenderGraph.hpp"

namespace RenderToy {

RenderGraphResources::RenderGraphResources(RenderGraph* graph)
    : m_graph(graph)
{
}

RHITextureHandle RenderGraphResources::getTexture(RGTextureHandle handle) const {
    return m_graph->getRHITexture(handle);
}

RHITextureHandle RenderGraphResources::getTexture(const std::string& name) const {
    return m_graph->getRHITexture(name);
}

RHIBufferHandle RenderGraphResources::getBuffer(RGBufferHandle handle) const {
    return m_graph->getRHIBuffer(handle);
}

RHIBufferHandle RenderGraphResources::getBuffer(const std::string& name) const {
    return m_graph->getRHIBuffer(name);
}

} // namespace RenderToy
