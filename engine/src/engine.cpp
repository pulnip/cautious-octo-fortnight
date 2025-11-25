#include <print>
#include "Engine.hpp"
#include "IGame.hpp"
#include "ECS/AnimationSystem.hpp"
#include "ECS/PhysicsSystem.hpp"
#include "ECS/RenderSystem.hpp"
#include "ECS/TransformSystem.hpp"
#include "ECS/UISystem.hpp"

#include "Log/Log.hpp"
#include "Log/ConsoleSink.hpp"
#include "Log/SDLSink.hpp"

namespace RenderToy
{
    int Engine::run(IGame& game){
        Logger::instance().addSink(std::make_unique<ConsoleSink>());
        LOG_DEBUG(LOG_CORE, "Engine Start");

        EngineConfig config;
        game.onConfigure(config);

        WindowDesc windowDesc{
            .title = config.windowTitle,
            .width = config.width,
            .height = config.height,
            .resizable = config.resizable
        };
        if(!window.init(windowDesc))
            return -1;

        world.addSystem<AnimationSystem>();
        world.addSystem<PhysicsSystem>();
        world.addSystem<RenderSystem>();
        world.addSystem<TransformSystem>();
        world.addSystem<UISystem>();

        game.onInit(world);

        timer.reset();

        bool isRunning = true;
        while(isRunning && !window.getShouldClose()){
            window.pumpEvents([&](const WindowEvent& event){
                if(event.type == WindowEvent::Type::Quit)
                    isRunning = false;
            });

            timer.newFrame();

            UpdateContext updateContext{
                .deltaTime = timer.deltaSeconds(),
            };
            game.onUpdate(updateContext);

            world.update(updateContext.deltaTime);

            // render
        }

        game.onShutdown();

        return 0;
    }
}