#pragma once

#include "ECS/ISystem.hpp"

namespace RenderToy
{
    class AnimationSystem: public ISystem{
    public:
        inline const char* getName() const override{
            return "AnimationSystem";
        }

        void onUpdate(DeltaTime) override;

        SystemChain execAfter() const override;
        SystemChain execBefore() const override;
    };
}