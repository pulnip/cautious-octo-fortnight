#pragma once

#include "generic_handle.hpp"
#include "RHI/RHITypes.hpp"
#include "RHI/RHIDevice.hpp"
#include "math.hpp"
#include "Resource/Submesh.hpp"
#include <vector>

namespace RenderToy
{
    // Forward declarations
    struct Texture;
    struct Material;
    struct Shader;
    struct Mesh;
    struct MaterialSet;

    // ============================================================================
    // Handle Type Aliases
    // ============================================================================

    using SubmeshHandle = generic_handle<Submesh>;  // Submesh defined in Submesh.hpp
    using TextureHandle = generic_handle<Texture>;
    using MaterialHandle = generic_handle<Material>;
    using ShaderHandle = generic_handle<Shader>;
    using MeshHandle = generic_handle<Mesh>;
    using MaterialSetHandle = generic_handle<MaterialSet>;

    // ============================================================================
    // GPU Resource Types
    // ============================================================================

    // Note: Submesh is defined in Submesh.hpp

    /**
     * GPU texture resource
     */
    struct Texture
    {
        RHITextureHandle handle;
        uint32_t width = 0;
        uint32_t height = 0;
        RHITextureFormat format = RHITextureFormat::RGBA8_UNORM;

        inline bool isValid() const {
            return handle.isValid();
        }
    };

    /**
     * Material properties + texture references
     * PBR-based material parameters
     */
    struct Material
    {
        // PBR parameters
        Vec4 albedo = Vec4{1.0f, 1.0f, 1.0f, 1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float alpha = 1.0f;

        // Texture maps
        TextureHandle albedoMap;
        TextureHandle normalMap;
        TextureHandle metallicRoughnessMap;
        TextureHandle emissiveMap;

        inline bool hasAlbedoMap() const { return albedoMap.isValid(); }
        inline bool hasNormalMap() const { return normalMap.isValid(); }
        inline bool hasMetallicRoughnessMap() const { return metallicRoughnessMap.isValid(); }
        inline bool hasEmissiveMap() const { return emissiveMap.isValid(); }
    };

    /**
     * Shader program
     */
    struct Shader
    {
        RHIShaderHandle vertexShader;
        RHIShaderHandle fragmentShader;

        inline bool isValid() const {
            return vertexShader.isValid() && fragmentShader.isValid();
        }
    };

    // ============================================================================
    // Logical Grouping Types
    // ============================================================================

    /**
     * Mesh = collection of submeshes
     * Logical grouping, not a GPU resource itself
     * Represents a complete 3D model (e.g., "car.gltf")
     */
    struct Mesh
    {
        std::vector<SubmeshHandle> submeshes;

        inline size_t submeshCount() const { return submeshes.size(); }
        inline bool empty() const { return submeshes.empty(); }
        inline bool isValid() const { return !submeshes.empty(); }
    };

    /**
     * MaterialSet = collection of materials
     * One material per submesh
     * Allows independent mesh/material combinations (e.g., team colors, skins)
     */
    struct MaterialSet
    {
        std::vector<MaterialHandle> materials;

        inline size_t materialCount() const { return materials.size(); }
        inline bool empty() const { return materials.empty(); }
        inline bool isValid() const { return !materials.empty(); }

        /**
         * Check if material count matches submesh count
         * @param mesh The mesh to compare against
         * @return true if counts match
         */
        inline bool matchesSubmeshCount(const Mesh& mesh) const {
            return materials.size() == mesh.submeshes.size();
        }
    };
}
