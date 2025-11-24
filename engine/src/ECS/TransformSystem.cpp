#include "ECS/TransformSystem.hpp"
#include "ECS/AnimationSystem.hpp"
#include "ECS/RenderSystem.hpp"

namespace RenderToy
{
    void TransformSystem::onUpdate(DeltaTime deltaTime){
        
    }

    ISystem::SystemChain TransformSystem::execAfter() const{
        return { typeid(AnimationSystem) };
    }
    ISystem::SystemChain TransformSystem::execBefore() const{
        return { typeid(RenderSystem) };
    }
}