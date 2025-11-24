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
    void Engine::onInit(IGame* game){
        Logger::instance().addSink(std::make_unique<ConsoleSink>());

        LOG_ERROR(LOG_CORE, "Engine Start");

        world.addSystem<AnimationSystem>();
        world.addSystem<PhysicsSystem>();
        world.addSystem<RenderSystem>();
        world.addSystem<TransformSystem>();
        world.addSystem<UISystem>();

        this->game = game;

        game->onInit(&world);
    }

    void Engine::onUpdate(DeltaTime deltaTime){
        game->onUpdate(deltaTime);

        world.update(deltaTime);
    }

    void Engine::onShutdown(){
        game->onShutdown();

        delete game;
    }
}