#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "Content/SceneFormat.hpp"

namespace RenderToy
{
    // ============================================================================
    // Scene Importer - TOML to SceneDescriptor
    // ============================================================================

    /// Import scene from TOML file
    /// @param filePath Path to .scene.toml file
    /// @return SceneDescriptor if successful, nullopt on error
    std::optional<SceneDescriptor> importScene(const std::string& filePath);

    /// Import scene from TOML string
    /// @param tomlText TOML content as string
    /// @return SceneDescriptor if successful, nullopt on error
    std::optional<SceneDescriptor> importSceneFromString(std::string_view tomlText);
}
