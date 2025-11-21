#pragma once

namespace RenderToy
{
    struct LogCategory{
        const char* const name;
    };

    inline constexpr LogCategory LOG_CORE { "Core" };
    inline constexpr LogCategory LOG_RENDER { "Render" };
    inline constexpr LogCategory LOG_SCENE { "Scene" };
}