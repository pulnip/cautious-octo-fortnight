#pragma once

#include "generic_handle.hpp"
#include "Resource/ResourceManager.hpp"
#include "Resource/RenderingResources.hpp"
#include "Resource/RenderingResourceTraits.hpp"
#include "Resource/Script.hpp"

namespace RenderToy
{
    // Rendering resource handles are defined in RenderingResources.hpp
    // SubmeshHandle, TextureHandle, MaterialHandle, ShaderHandle,
    // MeshHandle, MaterialSetHandle

    // Legacy compatibility (TODO: remove after migration)
    using ScriptHandle  = generic_handle<Script>;

    // Resource Managers
    using SubmeshManager    = ResourceManager<Submesh>;
    using TextureManager    = ResourceManager<Texture>;
    using MaterialManager   = ResourceManager<Material>;
    using ShaderManager     = ResourceManager<Shader>;
    using MeshManager       = ResourceManager<Mesh>;
    using MaterialSetManager = ResourceManager<MaterialSet>;
    using ScriptManager     = ResourceManager<Script>;
}