#include "ECS/PhysicsSystem.hpp"
#include "ECS/AnimationSystem.hpp"

namespace RenderToy
{
    void PhysicsSystem::onUpdate(DeltaTime deltaTime){
        
    }

    ISystem::SystemChain PhysicsSystem::execBefore() const{
        return { typeid(AnimationSystem) };
    }
}