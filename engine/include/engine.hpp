#pragma once

#include "ECS/World.hpp"
#include "Platform/Window.hpp"
#include "Time/Timer.hpp"

namespace RenderToy
{
    class IGame;

    class Engine{
    private:
        Window window;
        World world;
        Timer timer;

    public:
        int run(IGame&);
    };
}
