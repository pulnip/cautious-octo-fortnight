#include "ECS/AnimationSystem.hpp"
#include "ECS/PhysicsSystem.hpp"
#include "ECS/TransformSystem.hpp"

namespace RenderToy
{
    void AnimationSystem::onUpdate(DeltaTime deltaTime){
        
    }

    ISystem::SystemChain AnimationSystem::execAfter() const{
        return { typeid(PhysicsSystem) };
    }
    ISystem::SystemChain AnimationSystem::execBefore() const{
        return { typeid(TransformSystem) };
    }
}