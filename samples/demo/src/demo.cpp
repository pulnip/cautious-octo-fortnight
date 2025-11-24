#include <print>
#include "Demo.hpp"
#include "ECS/World.hpp"
#include "GameLogicSystem.hpp"
#include "InputSystem.hpp"

using RenderToy::EngineConfig;
using RenderToy::World, RenderToy::UpdateContext;

namespace Demo{
    void DemoGame::onConfigure(EngineConfig& config){
        config.windowTitle = "Demo App";
    }

    void DemoGame::onInit(World& world){
        std::println("Demo test");
        world.addSystem<GameLogicSystem>();
        world.addSystem<InputSystem>();
    }

    void DemoGame::onUpdate(const UpdateContext& context){

    }

    void DemoGame::onShutdown(){

    }
}