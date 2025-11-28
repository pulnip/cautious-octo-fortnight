#include "RenderGraph/RenderPass.hpp"
#include <algorithm>

namespace RenderToy {

RenderPass::RenderPass(std::string name, PassSetupFunc setupFunc, PassExecuteFunc executeFunc)
    : m_name(std::move(name))
    , m_setupFunc(std::move(setupFunc))
    , m_executeFunc(std::move(executeFunc))
{
}

void RenderPass::addDependency(ResourceDependency dep) {
    m_dependencies.push_back(dep);
}

std::vector<RGTextureHandle> RenderPass::getTextureReads() const {
    std::vector<RGTextureHandle> reads;
    for (const auto& dep : m_dependencies) {
        if (dep.isTexture() && (dep.accessMode == ResourceAccessMode::Read ||
                                 dep.accessMode == ResourceAccessMode::ReadWrite)) {
            reads.push_back(dep.textureHandle);
        }
    }
    return reads;
}

std::vector<RGTextureHandle> RenderPass::getTextureWrites() const {
    std::vector<RGTextureHandle> writes;
    for (const auto& dep : m_dependencies) {
        if (dep.isTexture() && (dep.accessMode == ResourceAccessMode::Write ||
                                 dep.accessMode == ResourceAccessMode::ReadWrite)) {
            writes.push_back(dep.textureHandle);
        }
    }
    return writes;
}

std::vector<RGBufferHandle> RenderPass::getBufferReads() const {
    std::vector<RGBufferHandle> reads;
    for (const auto& dep : m_dependencies) {
        if (dep.isBuffer() && (dep.accessMode == ResourceAccessMode::Read ||
                                dep.accessMode == ResourceAccessMode::ReadWrite)) {
            reads.push_back(dep.bufferHandle);
        }
    }
    return reads;
}

std::vector<RGBufferHandle> RenderPass::getBufferWrites() const {
    std::vector<RGBufferHandle> writes;
    for (const auto& dep : m_dependencies) {
        if (dep.isBuffer() && (dep.accessMode == ResourceAccessMode::Write ||
                                dep.accessMode == ResourceAccessMode::ReadWrite)) {
            writes.push_back(dep.bufferHandle);
        }
    }
    return writes;
}

} // namespace RenderToy
