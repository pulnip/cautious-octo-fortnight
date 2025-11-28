#pragma once

#include "ECS/World.hpp"
#include "Platform/Window.hpp"
#include "Time/Timer.hpp"
#include "RHI/RHIDevice.hpp"
#include "RHI/RHISwapchainHelpers.hpp"
#include "RHI/RHIFramePacing.hpp"
#include "RHI/RHICommandListPool.hpp"
#include "RHI/RHIPipelineCache.hpp"
#include <memory>

namespace RenderToy
{
    class IGame;

    class Engine{
    private:
        Window window;
        World world;
        Timer timer;

        // RHI resources
        std::unique_ptr<RHIDevice> rhiDevice;
        std::unique_ptr<RHISwapchainWrapper> swapchain;
        std::unique_ptr<RHIFramePacer> framePacer;
        std::unique_ptr<RHICommandListPool> commandPool;
        std::unique_ptr<RHIPipelineCache> pipelineCache;

    public:
        int run(IGame&);

        // RHI accessors
        RHIDevice* getRHIDevice() const { return rhiDevice.get(); }
        RHISwapchainWrapper* getSwapchain() const { return swapchain.get(); }
        RHIFramePacer* getFramePacer() const { return framePacer.get(); }
        RHICommandListPool* getCommandPool() const { return commandPool.get(); }
        RHIPipelineCache* getPipelineCache() const { return pipelineCache.get(); }
    };
}
