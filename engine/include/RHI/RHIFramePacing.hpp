#pragma once

#include "RHIFenceHelpers.hpp"
#include "RHISwapchain.hpp"
#include "RHIDevice.hpp"
#include "Log/Category.hpp"
#include "Log/Log.hpp"
#include <chrono>

namespace RenderToy
{
    // Frame pacing and synchronization system
    // Manages triple buffering, frame timing, and CPU-GPU synchronization
    class RHIFramePacer
    {
    public:
        explicit RHIFramePacer(RHIDevice* device)
            : device(device)
            , fenceManager(device)
            , frameNumber(0)
            , lastFrameTime(std::chrono::high_resolution_clock::now())
            , deltaTime(0.0)
            , fps(0.0)
            , frameTimeAccum(0.0)
            , frameCount(0)
        {
            LOG_INFO(LOG_RHI, "Frame pacer initialized");
        }

        // Begin a new frame
        // Returns true if ready to render, false if should skip
        bool beginFrame()
        {
            // Wait for oldest frame to complete
            fenceManager.beginFrame();

            // Update timing
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - lastFrameTime;
            deltaTime = elapsed.count();
            lastFrameTime = currentTime;

            // Update FPS counter
            frameTimeAccum += deltaTime;
            frameCount++;

            if (frameTimeAccum >= 1.0) {
                fps = static_cast<double>(frameCount) / frameTimeAccum;
                frameTimeAccum = 0.0;
                frameCount = 0;

                LOG_DEBUG(LOG_RHI, "FPS: {:.1f}, Frame Time: {:.2f}ms",
                         fps, deltaTime * 1000.0);
            }

            frameNumber++;
            return true;
        }

        // End the current frame
        void endFrame()
        {
            // Signal fence for this frame
            fenceManager.endFrame();
        }

        // Wait for all frames to complete
        void waitForIdle()
        {
            fenceManager.waitForAll();
            LOG_INFO(LOG_RHI, "Frame pacer idle");
        }

        // Get current frame index (0 to RHI_FRAMES_IN_FLIGHT-1)
        uint32_t getCurrentFrameIndex() const
        {
            return fenceManager.getCurrentFrameIndex();
        }

        // Get absolute frame number
        uint64_t getFrameNumber() const { return frameNumber; }

        // Get time since last frame (in seconds)
        double getDeltaTime() const { return deltaTime; }

        // Get current FPS
        double getFPS() const { return fps; }

        // Get frame time in milliseconds
        double getFrameTimeMs() const { return deltaTime * 1000.0; }

        // Get fence for current frame
        RHIFenceHandle getCurrentFence() const
        {
            return fenceManager.getCurrentFence();
        }

    private:
        RHIDevice* device;
        RHIFrameFenceManager fenceManager;

        uint64_t frameNumber;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;
        double deltaTime;
        double fps;
        double frameTimeAccum;
        uint32_t frameCount;
    };

    // Per-frame resource manager
    // Manages resources that need to be duplicated per frame in flight
    template<typename T>
    class PerFrameResource
    {
    public:
        PerFrameResource() = default;

        // Initialize with a factory function
        template<typename Factory>
        void initialize(Factory&& factory)
        {
            for (uint32_t i = 0; i < RHI_FRAMES_IN_FLIGHT; ++i) {
                resources[i] = factory(i);
            }
        }

        // Get resource for current frame
        T& get(uint32_t frameIndex)
        {
            return resources[frameIndex % RHI_FRAMES_IN_FLIGHT];
        }

        const T& get(uint32_t frameIndex) const
        {
            return resources[frameIndex % RHI_FRAMES_IN_FLIGHT];
        }

        // Get resource for current frame via pacer
        T& get(const RHIFramePacer& pacer)
        {
            return get(pacer.getCurrentFrameIndex());
        }

        const T& get(const RHIFramePacer& pacer) const
        {
            return get(pacer.getCurrentFrameIndex());
        }

        // Access all resources
        std::array<T, RHI_FRAMES_IN_FLIGHT>& getAll() { return resources; }
        const std::array<T, RHI_FRAMES_IN_FLIGHT>& getAll() const { return resources; }

        // Direct access by index
        T& operator[](uint32_t index) { return resources[index % RHI_FRAMES_IN_FLIGHT]; }
        const T& operator[](uint32_t index) const { return resources[index % RHI_FRAMES_IN_FLIGHT]; }

    private:
        std::array<T, RHI_FRAMES_IN_FLIGHT> resources;
    };

    // Example usage of per-frame resources:
    // PerFrameResource<RHIBufferHandle> constantBuffers;
    // constantBuffers.initialize([device](uint32_t frame) {
    //     return RHIBufferHelpers::createConstantBuffer(device, 256);
    // });
    //
    // auto& cb = constantBuffers.get(pacer);
    // device->updateBuffer(cb, &data, sizeof(data));
}
