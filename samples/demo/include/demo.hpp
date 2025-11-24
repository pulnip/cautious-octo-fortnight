#pragma once

#include "IGame.hpp"

namespace Demo{
    class DemoGame: public RenderToy::IGame{
    public:
        void onConfigure(RenderToy::EngineConfig&) override;
        void onInit(RenderToy::World&) override;
        void onUpdate(const RenderToy::UpdateContext&) override;
        void onShutdown() override;
    };
}