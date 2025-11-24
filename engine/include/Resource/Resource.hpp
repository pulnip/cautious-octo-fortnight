#pragma once

#include "generic_handle.hpp"
#include "Resource/ResourceManager.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Script.hpp"
#include "Resource/Shader.hpp"
#include "Resource/Texture.hpp"

namespace RenderToy
{
    using MeshHandle    = generic_handle<Mesh>;
    using ScriptHandle  = generic_handle<Script>;
    using ShaderHandle  = generic_handle<Shader>;
    using TextureHandle = generic_handle<Texture>;

    using MeshManager    = ResourceManager<Mesh>;
    using ScriptManager  = ResourceManager<Script>;
    using ShaderManager  = ResourceManager<Shader>;
    using TextureManager = ResourceManager<Texture>;
}