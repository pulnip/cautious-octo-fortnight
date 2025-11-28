#pragma once

#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "Log/Category.hpp"
#include "Log/Log.hpp"
#include <vector>
#include <mutex>

namespace RenderToy
{
    // Command list pool for efficient reuse of command lists
    // Thread-safe pool that manages command list lifecycle
    class RHICommandListPool{
    public:
        explicit RHICommandListPool(RHIDevice* device, uint32_t initialSize = 4)
            : device(device)
        {
            reserve(initialSize);
        }

        ~RHICommandListPool(){
            clear();
        }

        // Acquire a command list from the pool
        // Returns a command list ready to record commands
        RHICommandList* acquire()
        {
            std::lock_guard<std::mutex> lock(mutex);

            RHICommandList* cmdList = nullptr;

            // Try to reuse an available command list
            if (!available.empty()) {
                cmdList = available.back();
                available.pop_back();
                LOG_DEBUG(LOG_RHI, "Reusing command list from pool (available: {})", available.size());
            }
            else {
                // Create a new command list
                cmdList = device->beginCommandList();
                if (cmdList) {
                    all.push_back(cmdList);
                    LOG_INFO(LOG_RHI, "Created new command list (total: {})", all.size());
                }
            }

            if (cmdList) {
                // Begin recording
                cmdList->begin();
                inUse.push_back(cmdList);
            }

            return cmdList;
        }

        // Release a command list back to the pool
        // Command list must be closed before releasing
        void release(RHICommandList* cmdList)
        {
            if (!cmdList) return;

            std::lock_guard<std::mutex> lock(mutex);

            // Remove from in-use list
            auto it = std::find(inUse.begin(), inUse.end(), cmdList);
            if (it != inUse.end()) {
                inUse.erase(it);
                available.push_back(cmdList);
                LOG_DEBUG(LOG_RHI, "Released command list to pool (available: {})", available.size());
            }
        }

        // Release multiple command lists at once
        void release(const std::vector<RHICommandList*>& cmdLists)
        {
            std::lock_guard<std::mutex> lock(mutex);

            for (auto* cmdList : cmdLists) {
                if (!cmdList) continue;

                auto it = std::find(inUse.begin(), inUse.end(), cmdList);
                if (it != inUse.end()) {
                    inUse.erase(it);
                    available.push_back(cmdList);
                }
            }

            LOG_DEBUG(LOG_RHI, "Released {} command lists to pool (available: {})",
                     cmdLists.size(), available.size());
        }

        // Reserve space for command lists
        void reserve(uint32_t count)
        {
            std::lock_guard<std::mutex> lock(mutex);

            uint32_t currentSize = static_cast<uint32_t>(all.size());
            if (count > currentSize) {
                all.reserve(count);
                available.reserve(count);
            }
        }

        // Clear all command lists
        void clear()
        {
            std::lock_guard<std::mutex> lock(mutex);

            // Wait for all in-use command lists to finish
            if (!inUse.empty()) {
                LOG_WARN(LOG_RHI, "Clearing pool with {} command lists still in use", inUse.size());
            }

            // Clean up all command lists
            for (auto* cmdList : all) {
                // Cleanup is handled by RHIDevice
                // Command lists are destroyed when device is destroyed
            }

            all.clear();
            available.clear();
            inUse.clear();

            LOG_INFO(LOG_RHI, "Command list pool cleared");
        }

        // Get statistics
        struct Statistics {
            uint32_t total = 0;
            uint32_t available = 0;
            uint32_t inUse = 0;
        };

        Statistics getStatistics() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            Statistics stats;
            stats.total = static_cast<uint32_t>(all.size());
            stats.available = static_cast<uint32_t>(available.size());
            stats.inUse = static_cast<uint32_t>(inUse.size());
            return stats;
        }

        void logStatistics() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            LOG_INFO(LOG_RHI, "Command List Pool Statistics:");
            LOG_INFO(LOG_RHI, "  Total: {}", all.size());
            LOG_INFO(LOG_RHI, "  Available: {}", available.size());
            LOG_INFO(LOG_RHI, "  In Use: {}", inUse.size());
        }

    private:
        RHIDevice* device;

        std::vector<RHICommandList*> all;        // All command lists owned by pool
        std::vector<RHICommandList*> available;  // Available for reuse
        std::vector<RHICommandList*> inUse;      // Currently being recorded

        mutable std::mutex mutex;
    };

    // RAII wrapper for automatic command list acquisition and release
    class ScopedCommandList
    {
    public:
        ScopedCommandList(RHICommandListPool* pool)
            : pool(pool)
            , cmdList(pool ? pool->acquire() : nullptr)
        {
        }

        ~ScopedCommandList()
        {
            if (cmdList && pool) {
                cmdList->close();
                pool->release(cmdList);
            }
        }

        // No copy
        ScopedCommandList(const ScopedCommandList&) = delete;
        ScopedCommandList& operator=(const ScopedCommandList&) = delete;

        // Move support
        ScopedCommandList(ScopedCommandList&& other) noexcept
            : pool(other.pool)
            , cmdList(other.cmdList)
        {
            other.pool = nullptr;
            other.cmdList = nullptr;
        }

        ScopedCommandList& operator=(ScopedCommandList&& other) noexcept
        {
            if (this != &other) {
                if (cmdList && pool) {
                    cmdList->close();
                    pool->release(cmdList);
                }

                pool = other.pool;
                cmdList = other.cmdList;

                other.pool = nullptr;
                other.cmdList = nullptr;
            }
            return *this;
        }

        RHICommandList* get() const { return cmdList; }
        RHICommandList* operator->() const { return cmdList; }
        operator RHICommandList*() const { return cmdList; }

        bool isValid() const { return cmdList != nullptr; }

    private:
        RHICommandListPool* pool;
        RHICommandList* cmdList;
    };
}
