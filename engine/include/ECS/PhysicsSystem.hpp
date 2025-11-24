#pragma once

#include "ECS/ISystem.hpp"

namespace RenderToy
{
    class PhysicsSystem: public ISystem{
    public:
        inline const char* getName() const override{
            return "PhysicsSystem";
        }

        void onUpdate(DeltaTime) override;

        SystemChain execBefore() const override;
    };
}