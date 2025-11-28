#pragma once

#include <unordered_map>
#include <mutex>
#include <string>
#include "RHI/RHIPlatform.h"
#include "Log/Log.hpp"

namespace RenderToy
{
    // Resource leak detection tracker
    // Tracks all RHI resource allocations and deallocations
    // Only enabled in debug builds (RHI_ENABLE_RESOURCE_TRACKING)
    class ResourceTracker
    {
#if RHI_ENABLE_RESOURCE_TRACKING
    private:
        struct ResourceEntry
        {
            std::string debugName;
            std::string typeName;
            size_t sizeBytes;
            void* nativeResource;
        };

        std::unordered_map<void*, ResourceEntry> liveResources;
        std::mutex mutex;
        size_t totalAllocatedBytes = 0;

        static ResourceTracker& instance() {
            static ResourceTracker tracker;
            return tracker;
        }

        ResourceTracker() = default;

    public:
        // Track resource creation
        static void trackCreate(void* handle,
                               const char* typeName,
                               const char* debugName,
                               size_t sizeBytes,
                               void* nativeResource = nullptr)
        {
            auto& tracker = instance();
            std::lock_guard<std::mutex> lock(tracker.mutex);

            tracker.liveResources[handle] = ResourceEntry{
                .debugName = debugName ? debugName : "Unnamed",
                .typeName = typeName,
                .sizeBytes = sizeBytes,
                .nativeResource = nativeResource
            };

            tracker.totalAllocatedBytes += sizeBytes;

            LOG_DEBUG(LOG_RHI, "Created {} '{}' ({} bytes, handle={:p})",
                     typeName, debugName ? debugName : "Unnamed", sizeBytes, handle);
        }

        // Track resource destruction
        static void trackDestroy(void* handle)
        {
            auto& tracker = instance();
            std::lock_guard<std::mutex> lock(tracker.mutex);

            auto it = tracker.liveResources.find(handle);
            if (it != tracker.liveResources.end()) {
                tracker.totalAllocatedBytes -= it->second.sizeBytes;

                LOG_DEBUG(LOG_RHI, "Destroyed {} '{}' ({} bytes, handle={:p})",
                         it->second.typeName.c_str(),
                         it->second.debugName.c_str(),
                         it->second.sizeBytes,
                         handle);

                tracker.liveResources.erase(it);
            } else {
                LOG_WARN(LOG_RHI, "Attempted to destroy unknown resource (handle={:p})", handle);
            }
        }

        // Dump all live resources (for leak detection)
        static void dumpLeaks()
        {
            auto& tracker = instance();
            std::lock_guard<std::mutex> lock(tracker.mutex);

            if (tracker.liveResources.empty()) {
                LOG_INFO(LOG_RHI, "No resource leaks detected. All resources properly destroyed.");
                return;
            }

            LOG_ERROR(LOG_RHI, "RESOURCE LEAKS DETECTED: {} live resources, {} bytes total",
                     tracker.liveResources.size(), tracker.totalAllocatedBytes);

            for (const auto& [handle, entry] : tracker.liveResources) {
                LOG_ERROR(LOG_RHI, "  LEAK: {} '{}' ({} bytes, handle={:p}, native={:p})",
                         entry.typeName.c_str(),
                         entry.debugName.c_str(),
                         entry.sizeBytes,
                         handle,
                         entry.nativeResource);
            }
        }

        // Get statistics
        static size_t getLiveResourceCount()
        {
            auto& tracker = instance();
            std::lock_guard<std::mutex> lock(tracker.mutex);
            return tracker.liveResources.size();
        }

        static size_t getTotalAllocatedBytes()
        {
            auto& tracker = instance();
            std::lock_guard<std::mutex> lock(tracker.mutex);
            return tracker.totalAllocatedBytes;
        }

#else
        // No-op in release builds
    public:
        static void trackCreate(void*, const char*, const char*, size_t, void* = nullptr) {}
        static void trackDestroy(void*) {}
        static void dumpLeaks() {}
        static size_t getLiveResourceCount() { return 0; }
        static size_t getTotalAllocatedBytes() { return 0; }
#endif
    };
}
