#pragma once

#include "ECS/ISystem.hpp"

namespace Demo
{
    class InputSystem: public RenderToy::ISystem{
        inline const char* getName() const override{
            return "InputSystem";
        }

        void onUpdate(RenderToy::DeltaTime) override;

        SystemChain execBefore() const override;
    };
}