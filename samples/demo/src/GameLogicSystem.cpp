#include "GameLogicSystem.hpp"
#include "InputSystem.hpp"
#include "ECS/PhysicsSystem.hpp"

using RenderToy::DeltaTime, RenderToy::ISystem;
using RenderToy::PhysicsSystem;

namespace Demo
{
    void GameLogicSystem::onUpdate(DeltaTime deltaTime){
        
    }

    ISystem::SystemChain GameLogicSystem::execAfter() const{
        return { typeid(InputSystem) };
    }
    ISystem::SystemChain GameLogicSystem::execBefore() const{
        return { typeid(PhysicsSystem) };
    }
}