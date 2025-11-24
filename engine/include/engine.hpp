#pragma once

#include "ECS/World.hpp"
#include "Platform/Window.hpp"

namespace RenderToy
{
    class IGame;

    class Engine{
    private:
        Window window;
        World world;

    public:
        int run(IGame&);
    };
}
