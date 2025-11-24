#pragma once

#include "Time.hpp"

namespace RenderToy
{
    class World;

    class IGame{
    public:
        virtual ~IGame() = default;

        virtual void onInit(World*){}
        virtual void onUpdate(DeltaTime) = 0;
        virtual void onShutdown(){}
    };
}