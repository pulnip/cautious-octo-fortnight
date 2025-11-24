#pragma once

#include "IGame.hpp"

namespace Demo{
    class DemoGame: public RenderToy::IGame{
    public:
        void onInit(RenderToy::World*);
        void onUpdate(RenderToy::DeltaTime);
        void onShutdown();
    };
}