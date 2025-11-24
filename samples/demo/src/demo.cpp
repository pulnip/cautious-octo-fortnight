#include <print>
#include "Demo.hpp"
#include "ECS/World.hpp"
#include "GameLogicSystem.hpp"
#include "InputSystem.hpp"

using RenderToy::World, RenderToy::DeltaTime;

namespace Demo{
    void DemoGame::onInit(World* world){
        std::println("Demo test");
        world->addSystem<GameLogicSystem>();
        world->addSystem<InputSystem>();
    }

    void DemoGame::onUpdate(DeltaTime deltaTime){

    }

    void DemoGame::onShutdown(){

    }
}