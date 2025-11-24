#pragma once

#include "ISystem.hpp"

namespace RenderToy
{
    class RenderSystem: public ISystem{
    public:
        RenderSystem() = default;

        inline const char* getName() const override{
            return "RenderSystem";
        }

        void onUpdate(DeltaTime) override;

        SystemChain execAfter() const override;
    };
}