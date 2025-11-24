#include "ECS/UISystem.hpp"
#include "ECS/RenderSystem.hpp"

namespace RenderToy
{
    void UISystem::onUpdate(DeltaTime deltaTime){
        
    }

    ISystem::SystemChain UISystem::execAfter() const{
        return { typeid(RenderSystem) };
    }
}