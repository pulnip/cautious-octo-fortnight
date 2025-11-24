#include "InputSystem.hpp"
#include "GameLogicSystem.hpp"

using RenderToy::DeltaTime, RenderToy::ISystem;

namespace Demo
{
    void InputSystem::onUpdate(DeltaTime deltaTime){
        
    }

    ISystem::SystemChain InputSystem::execBefore() const{
        return { typeid(GameLogicSystem) };
    }
}