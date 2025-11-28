#pragma once

#include "RHIFence.hpp"
#include "RHIDevice.hpp"
#include "RHITypes.hpp"
#include "Log/Category.hpp"
#include "Log/Log.hpp"
#include <array>
#include <chrono>

namespace RenderToy
{
    // Helper class for managing per-frame fences
    // Used for triple buffering to synchronize CPU and GPU work
    class RHIFrameFenceManager
    {
    public:
        explicit RHIFrameFenceManager(RHIDevice* device)
            : device(device)
            , currentFrame(0)
        {
            // Create fences for each frame in flight
            for (uint32_t i = 0; i < RHI_FRAMES_IN_FLIGHT; ++i) {
                fences[i] = device->createFence(0);
                fenceValues[i] = 0;
            }

            LOG_INFO(LOG_RHI, "Created frame fence manager with {} frames in flight",
                     RHI_FRAMES_IN_FLIGHT);
        }

        ~RHIFrameFenceManager()
        {
            // Wait for all fences before destruction
            waitForAll();

            for (uint32_t i = 0; i < RHI_FRAMES_IN_FLIGHT; ++i) {
                if (fences[i].isValid()) {
                    device->destroyFence(fences[i]);
                }
            }
        }

        // Begin a new frame
        // Waits for the fence of the oldest frame to ensure GPU is done with it
        void beginFrame()
        {
            // Get the fence for the current frame
            RHIFenceHandle fence = fences[currentFrame];
            uint64_t targetValue = fenceValues[currentFrame];

            // Wait for GPU to finish with this frame's resources
            if (targetValue > 0) {
                device->waitForFence(fence, targetValue);
            }
        }

        // End the current frame
        // Signals the fence so GPU can indicate when work is done
        void endFrame()
        {
            // Increment fence value for next wait
            fenceValues[currentFrame]++;

            // Signal the fence with the new value
            device->signalFence(fences[currentFrame], fenceValues[currentFrame]);

            // Move to next frame
            currentFrame = (currentFrame + 1) % RHI_FRAMES_IN_FLIGHT;
        }

        // Wait for all frames to complete
        void waitForAll()
        {
            for (uint32_t i = 0; i < RHI_FRAMES_IN_FLIGHT; ++i) {
                if (fenceValues[i] > 0) {
                    device->waitForFence(fences[i], fenceValues[i]);
                }
            }

            LOG_INFO(LOG_RHI, "Waited for all {} frames to complete", RHI_FRAMES_IN_FLIGHT);
        }

        // Get current frame index
        uint32_t getCurrentFrameIndex() const { return currentFrame; }

        // Get fence for current frame
        RHIFenceHandle getCurrentFence() const { return fences[currentFrame]; }

        // Get fence value for current frame
        uint64_t getCurrentFenceValue() const { return fenceValues[currentFrame]; }

    private:
        RHIDevice* device;
        std::array<RHIFenceHandle, RHI_FRAMES_IN_FLIGHT> fences;
        std::array<uint64_t, RHI_FRAMES_IN_FLIGHT> fenceValues;
        uint32_t currentFrame;
    };

    // Simple fence wrapper for one-shot synchronization
    class RHISyncFence
    {
    public:
        explicit RHISyncFence(RHIDevice* device, uint64_t initialValue = 0)
            : device(device)
            , fence(device->createFence(initialValue))
            , nextValue(initialValue + 1)
        {
        }

        ~RHISyncFence()
        {
            if (fence.isValid()) {
                device->destroyFence(fence);
            }
        }

        // Signal the fence
        void signal()
        {
            device->signalFence(fence, nextValue);
            lastSignaledValue = nextValue;
            nextValue++;
        }

        // Wait for the fence to reach the last signaled value
        void wait()
        {
            if (lastSignaledValue > 0) {
                device->waitForFence(fence, lastSignaledValue);
            }
        }

        // Wait for a specific value
        void waitForValue(uint64_t value)
        {
            device->waitForFence(fence, value);
        }

        // Check if fence has reached a value (non-blocking)
        bool isComplete(uint64_t value) const
        {
            return device->isFenceComplete(fence, value);
        }

        // Check if last signaled value is complete
        bool isComplete() const
        {
            return lastSignaledValue > 0 && device->isFenceComplete(fence, lastSignaledValue);
        }

        RHIFenceHandle getHandle() const { return fence; }
        uint64_t getLastSignaledValue() const { return lastSignaledValue; }

    private:
        RHIDevice* device;
        RHIFenceHandle fence;
        uint64_t nextValue;
        uint64_t lastSignaledValue = 0;
    };

    // Fence helpers for common synchronization patterns
    namespace RHIFenceHelpers
    {
        // Create and immediately signal a fence
        inline RHIFenceHandle createSignaledFence(RHIDevice* device, uint64_t value = 1)
        {
            RHIFenceHandle fence = device->createFence(value);
            device->signalFence(fence, value);
            return fence;
        }

        // Wait for multiple fences
        inline void waitForFences(
            RHIDevice* device,
            const std::vector<RHIFenceHandle>& fences,
            const std::vector<uint64_t>& values)
        {
            if (fences.size() != values.size()) {
                LOG_ERROR(LOG_RHI, "Fence count mismatch in waitForFences");
                return;
            }

            for (size_t i = 0; i < fences.size(); ++i) {
                device->waitForFence(fences[i], values[i]);
            }
        }
    }
}
