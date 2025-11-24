#pragma once

#include "ISystem.hpp"

namespace RenderToy
{
    class TransformSystem: public ISystem{
    public:
        TransformSystem() = default;

        inline const char* getName() const override{
            return "TransformSystem";
        }

        void onUpdate(DeltaTime) override;

        SystemChain execAfter() const override;
        SystemChain execBefore() const override;
    };
}