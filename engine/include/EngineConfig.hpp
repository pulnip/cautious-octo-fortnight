#pragma once

namespace RenderToy
{
    struct EngineConfig{
        const char* windowTitle = "RenderToy";
        int width = 800;
        int height = 600;
        bool resizable = true;
    };
}