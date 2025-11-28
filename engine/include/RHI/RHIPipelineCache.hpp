#pragma once

#include "RHIDefinitions.h"
#include "RHITypes.hpp"
#include "RHIDevice.hpp"
#include "Log/Category.hpp"
#include "Log/Log.hpp"
#include <unordered_map>
#include <functional>
#include <mutex>

namespace RenderToy
{
    // Hash function for graphics pipeline state descriptor
    struct RHIGraphicsPipelineStateDescHash
    {
        size_t operator()(const RHIGraphicsPipelineStateDesc& desc) const noexcept
        {
            size_t hash = 0;

            // Hash vertex shader (simple hash using handle value)
            hash ^= std::hash<uint64_t>{}(reinterpret_cast<uint64_t>(&desc.vertexShaderIndex))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash pixel shader
            hash ^= std::hash<uint64_t>{}(reinterpret_cast<uint64_t>(&desc.pixelShaderIndex))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash vertex layout
            hash ^= std::hash<uint32_t>{}(desc.vertexLayout.elementCount)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash primitive topology
            hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(desc.topology))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash rasterizer state (simplified)
            hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(desc.rasterizer.fillMode))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(desc.rasterizer.cullMode))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash depth-stencil state
            hash ^= std::hash<bool>{}(desc.depthStencil.depthEnable)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<bool>{}(desc.depthStencil.depthWriteEnable)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash blend state (first render target only)
            hash ^= std::hash<bool>{}(desc.blend.renderTargets[0].blendEnable)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // Hash render target formats
            for (uint32_t i = 0; i < desc.renderTargetCount; ++i) {
                hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(desc.renderTargetFormats[i]))
                    + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            hash ^= std::hash<uint32_t>{}(static_cast<uint32_t>(desc.depthStencilFormat))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            return hash;
        }
    };

    // Equality comparison for graphics pipeline state descriptor
    struct RHIGraphicsPipelineStateDescEqual
    {
        bool operator()(const RHIGraphicsPipelineStateDesc& lhs,
                       const RHIGraphicsPipelineStateDesc& rhs) const noexcept
        {
            // Simplified comparison for now
            // Full comparison will be implemented when needed

            // Compare shaders
            if (lhs.vertexShaderIndex != rhs.vertexShaderIndex) return false;
            if (lhs.pixelShaderIndex != rhs.pixelShaderIndex) return false;

            // Compare topology
            if (lhs.topology != rhs.topology) return false;

            // Compare rasterizer state (simplified)
            if (lhs.rasterizer.fillMode != rhs.rasterizer.fillMode) return false;
            if (lhs.rasterizer.cullMode != rhs.rasterizer.cullMode) return false;

            // Compare depth-stencil state
            if (lhs.depthStencil.depthEnable != rhs.depthStencil.depthEnable) return false;
            if (lhs.depthStencil.depthWriteEnable != rhs.depthStencil.depthWriteEnable) return false;

            // Compare render target formats
            if (lhs.renderTargetCount != rhs.renderTargetCount) return false;
            for (uint32_t i = 0; i < lhs.renderTargetCount; ++i) {
                if (lhs.renderTargetFormats[i] != rhs.renderTargetFormats[i]) return false;
            }
            if (lhs.depthStencilFormat != rhs.depthStencilFormat) return false;

            return true;
        }
    };

    // Pipeline state object cache
    // Thread-safe cache for reusing pipeline state objects
    class RHIPipelineCache
    {
    public:
        explicit RHIPipelineCache(RHIDevice* device)
            : device(device)
        {
        }

        ~RHIPipelineCache()
        {
            clear();
        }

        // Get or create a graphics pipeline state
        RHIPipelineStateHandle getOrCreate(const RHIGraphicsPipelineStateDesc& desc)
        {
            std::lock_guard<std::mutex> lock(mutex);

            auto it = cache.find(desc);
            if (it != cache.end()) {
                // Cache hit
                ++stats.hits;
                LOG_DEBUG(LOG_RHI, "PSO cache hit (total hits: {})", stats.hits);
                return it->second;
            }

            // Cache miss - create new PSO
            ++stats.misses;
            LOG_INFO(LOG_RHI, "PSO cache miss - creating new PSO (total misses: {})", stats.misses);

            RHIPipelineStateHandle pso = device->createGraphicsPipelineState(desc);
            if (pso.isValid()) {
                cache[desc] = pso;
                ++stats.totalPSOs;

                LOG_INFO(LOG_RHI, "Created new PSO - total cached: {}", stats.totalPSOs);
            }

            return pso;
        }

        // Clear all cached pipeline states
        void clear()
        {
            std::lock_guard<std::mutex> lock(mutex);

            for (auto& [desc, handle] : cache) {
                if (handle.isValid()) {
                    device->destroyPipelineState(handle);
                }
            }

            cache.clear();
            stats = {};

            LOG_INFO(LOG_RHI, "PSO cache cleared");
        }

        // Get cache statistics
        struct Statistics {
            uint32_t hits = 0;
            uint32_t misses = 0;
            uint32_t totalPSOs = 0;

            float hitRate() const {
                uint32_t total = hits + misses;
                return total > 0 ? static_cast<float>(hits) / total : 0.0f;
            }
        };

        Statistics getStatistics() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return stats;
        }

        // Log cache statistics
        void logStatistics() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            LOG_INFO(LOG_RHI, "PSO Cache Statistics:");
            LOG_INFO(LOG_RHI, "  Total PSOs: {}", stats.totalPSOs);
            LOG_INFO(LOG_RHI, "  Cache Hits: {}", stats.hits);
            LOG_INFO(LOG_RHI, "  Cache Misses: {}", stats.misses);
            LOG_INFO(LOG_RHI, "  Hit Rate: {:.2f}%", stats.hitRate() * 100.0f);
        }

    private:
        RHIDevice* device;
        std::unordered_map<
            RHIGraphicsPipelineStateDesc,
            RHIPipelineStateHandle,
            RHIGraphicsPipelineStateDescHash,
            RHIGraphicsPipelineStateDescEqual
        > cache;

        mutable std::mutex mutex;
        Statistics stats;
    };
}
