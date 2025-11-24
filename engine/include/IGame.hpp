#pragma once

#include "UpdateContext.hpp"
#include "EngineConfig.hpp"

namespace RenderToy
{
    class World;

    class IGame{
    public:
        virtual ~IGame() = default;

        virtual void onConfigure(EngineConfig&){}
        virtual void onInit(World&){}
        virtual void onUpdate(const UpdateContext&) = 0;
        virtual void onShutdown(){}
    };
}