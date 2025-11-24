#pragma once

#include "ECS/ISystem.hpp"

namespace Demo
{
    class GameLogicSystem: public RenderToy::ISystem{
        inline const char* getName() const override{
            return "GameLogicSystem";
        }

        void onUpdate(RenderToy::DeltaTime) override;

        SystemChain execAfter() const override;
        SystemChain execBefore() const override;
    };
}