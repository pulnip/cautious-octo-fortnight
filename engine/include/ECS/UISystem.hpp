#pragma once

#include "ECS/ISystem.hpp"

namespace RenderToy
{
    class UISystem: public ISystem{
    public:
        inline const char* getName() const override{
            return "UISystem";
        }

        void onUpdate(DeltaTime) override;

        SystemChain execAfter() const override;
    };
}