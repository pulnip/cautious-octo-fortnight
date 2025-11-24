#pragma once

#include "ECS/World.hpp"

namespace RenderToy
{
    class IGame;

    class Engine{
    private:
        World world;
        IGame* game;

    public:
        void onInit(IGame*);
        void onUpdate(DeltaTime);
        void onShutdown();
    };
}
