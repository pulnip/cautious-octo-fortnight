#pragma once

namespace RenderToy
{
    struct LogCategory{
        const char* const name;
    };

    inline constexpr LogCategory LOG_CORE { "Core" };
    inline constexpr LogCategory LOG_RENDER { "Render" };
    inline constexpr LogCategory LOG_SCENE { "Scene" };
    inline constexpr LogCategory LOG_RESOURCE { "Resource" };
    inline constexpr LogCategory LOG_RHI { "RHI" };
    inline constexpr LogCategory LOG_D3D12 { "D3D12" };
    inline constexpr LogCategory LOG_METAL { "Metal" };
}