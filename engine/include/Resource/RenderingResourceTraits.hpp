#pragma once

#include <filesystem>
#include <string>
#include "Resource/ResourceTraits.hpp"
#include "Resource/RenderingResources.hpp"
#include "Resource/SubmeshTraits.hpp"

namespace RenderToy
{
    // Note: Submesh traits are defined in SubmeshTraits.hpp

    // ============================================================================
    // Texture Traits
    // ============================================================================

    struct TextureRequest {
        std::filesystem::path filePath;
    };

    struct TextureKey {
        std::string canonicalPath;

        auto operator<=>(const TextureKey&) const = default;
    };

    struct TextureKeyHash {
        inline size_t operator()(const TextureKey& k) const noexcept {
            return std::hash<std::string>{}(k.canonicalPath);
        }
    };

    template<>
    struct ResourceTraits<Texture> {
        using Request = TextureRequest;
        using Key     = TextureKey;
        using KeyHash = TextureKeyHash;

        inline static Key makeKey(const Request& request) {
            auto canonical = std::filesystem::weakly_canonical(request.filePath);
            return Key{
                .canonicalPath = canonical.string()
            };
        }

        // Note: Texture loading will be implemented later
        inline static Texture load(const Request& request) {
            // TODO: Implement texture loading with stb_image or similar
            return Texture{};
        }
    };

    // ============================================================================
    // Material Traits
    // ============================================================================

    struct MaterialRequest {
        std::filesystem::path filePath;
        uint32_t materialIndex;  // Index of material within the file
    };

    struct MaterialKey {
        std::string canonicalPath;
        uint32_t materialIndex;

        auto operator<=>(const MaterialKey&) const = default;
    };

    struct MaterialKeyHash {
        inline size_t operator()(const MaterialKey& k) const noexcept {
            size_t h1 = std::hash<std::string>{}(k.canonicalPath);
            size_t h2 = std::hash<uint32_t>{}(k.materialIndex);
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct ResourceTraits<Material> {
        using Request = MaterialRequest;
        using Key     = MaterialKey;
        using KeyHash = MaterialKeyHash;

        inline static Key makeKey(const Request& request) {
            auto canonical = std::filesystem::weakly_canonical(request.filePath);
            return Key{
                .canonicalPath = canonical.string(),
                .materialIndex = request.materialIndex
            };
        }

        // Note: load() is not implemented here
        // Material loading is handled by SceneLoader
        inline static Material load(const Request& request) {
            // Materials are loaded manually by SceneLoader
            return Material{};
        }
    };

    // ============================================================================
    // Shader Traits
    // ============================================================================

    struct ShaderRequest {
        std::filesystem::path vertexShaderPath;
        std::filesystem::path fragmentShaderPath;
    };

    struct ShaderKey {
        std::string vertexShaderCanonical;
        std::string fragmentShaderCanonical;

        auto operator<=>(const ShaderKey&) const = default;
    };

    struct ShaderKeyHash {
        inline size_t operator()(const ShaderKey& k) const noexcept {
            size_t h1 = std::hash<std::string>{}(k.vertexShaderCanonical);
            size_t h2 = std::hash<std::string>{}(k.fragmentShaderCanonical);
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct ResourceTraits<Shader> {
        using Request = ShaderRequest;
        using Key     = ShaderKey;
        using KeyHash = ShaderKeyHash;

        inline static Key makeKey(const Request& request) {
            auto vsCanonical = std::filesystem::weakly_canonical(request.vertexShaderPath);
            auto fsCanonical = std::filesystem::weakly_canonical(request.fragmentShaderPath);
            return Key{
                .vertexShaderCanonical = vsCanonical.string(),
                .fragmentShaderCanonical = fsCanonical.string()
            };
        }

        // Note: Shader loading will be implemented later
        inline static Shader load(const Request& request) {
            // TODO: Implement shader compilation
            return Shader{};
        }
    };

    // ============================================================================
    // Mesh Traits (New Definition - Collection of Submeshes)
    // ============================================================================

    struct MeshRequest {
        std::filesystem::path filePath;
    };

    struct MeshKey {
        std::string canonicalPath;

        auto operator<=>(const MeshKey&) const = default;
    };

    struct MeshKeyHash {
        inline size_t operator()(const MeshKey& k) const noexcept {
            return std::hash<std::string>{}(k.canonicalPath);
        }
    };

    template<>
    struct ResourceTraits<Mesh> {
        using Request = MeshRequest;
        using Key     = MeshKey;
        using KeyHash = MeshKeyHash;

        inline static Key makeKey(const Request& request) {
            auto canonical = std::filesystem::weakly_canonical(request.filePath);
            return Key{
                .canonicalPath = canonical.string()
            };
        }

        // Note: Mesh is a logical grouping of submeshes
        // Loading is handled by SceneLoader which creates submeshes first
        inline static Mesh load(const Request& request) {
            // Meshes are loaded manually by SceneLoader
            return Mesh{};
        }
    };

    // ============================================================================
    // MaterialSet Traits
    // ============================================================================

    struct MaterialSetRequest {
        std::filesystem::path filePath;
    };

    struct MaterialSetKey {
        std::string canonicalPath;

        auto operator<=>(const MaterialSetKey&) const = default;
    };

    struct MaterialSetKeyHash {
        inline size_t operator()(const MaterialSetKey& k) const noexcept {
            return std::hash<std::string>{}(k.canonicalPath);
        }
    };

    template<>
    struct ResourceTraits<MaterialSet> {
        using Request = MaterialSetRequest;
        using Key     = MaterialSetKey;
        using KeyHash = MaterialSetKeyHash;

        inline static Key makeKey(const Request& request) {
            auto canonical = std::filesystem::weakly_canonical(request.filePath);
            return Key{
                .canonicalPath = canonical.string()
            };
        }

        // Note: MaterialSet is a logical grouping of materials
        // Loading is handled by SceneLoader
        inline static MaterialSet load(const Request& request) {
            // MaterialSets are loaded manually by SceneLoader
            return MaterialSet{};
        }
    };
}
