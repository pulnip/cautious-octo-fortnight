#include "ECS/RenderSystem.hpp"
#include "ECS/TransformSystem.hpp"

namespace RenderToy
{
    void RenderSystem::onUpdate(DeltaTime deltaTime){

    }

    ISystem::SystemChain RenderSystem::execAfter() const{
        return { typeid(TransformSystem) };
    }
}