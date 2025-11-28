#pragma once

#include <optional>
#include <string>
#include "Content/MeshFormat.hpp"

namespace RenderToy
{
    // ============================================================================
    // Mesh Importer - 3D Model Files to MeshData
    // ============================================================================

    /// Import mesh from file (OBJ, FBX, glTF, etc.) using Assimp
    /// @param filePath Path to 3D model file
    /// @return MeshData if successful, nullopt on error
    std::optional<MeshData> importMesh(const std::string& filePath);

    /// Load embedded primitive mesh (cube, sphere, etc.)
    /// @param name Primitive name: "cube", "sphere", "plane"
    /// @return MeshData for the primitive
    std::optional<MeshData> loadEmbeddedMesh(const std::string& name);
}
