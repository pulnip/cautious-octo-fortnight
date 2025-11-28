#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include "Log/Log.hpp"
#include "Resource/ResourceTraits.hpp"
#include "Resource/Texture.hpp"

namespace RenderToy
{
    enum class TextureFormat{
        RGBA8,
    };

    enum class SamplingMode{
        Nearest,
        Linear,
    };

    struct TextureRequest{
        std::filesystem::path path;
        TextureFormat format = TextureFormat::RGBA8;
        SamplingMode sampling = SamplingMode::Linear;
        bool srgb = true;
    };

    struct TextureKey{
        std::string canonicalPath;
        TextureFormat format;
        SamplingMode sampling;
        bool srgb;

        auto operator<=>(const TextureKey&) const = default;
    };

    struct TextureKeyHash{
        inline size_t operator()(const TextureKey& k) const noexcept{
            size_t hash = std::hash<std::string>{}(k.canonicalPath);
            hash ^= (std::hash<int>{}(static_cast<int>(k.format))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2));
            hash ^= (std::hash<int>{}(static_cast<int>(k.sampling))
                + 0x9e3779b9 + (hash << 6) + (hash >> 2));
            hash ^= (std::hash<bool>{}(k.srgb)
                + 0x9e3779b9 + (hash << 6) + (hash >> 2));
            return hash;
        }
    };

    template<>
    struct ResourceTraits<Texture>{
        using Request = TextureRequest;
        using Key     = TextureKey;
        using KeyHash = TextureKeyHash;

        inline static Key makeKey(const Request& request){
            auto canonical = std::filesystem::weakly_canonical(request.path);

            return Key{
                .canonicalPath = canonical.string(),
                .format = request.format,
                .sampling = request.sampling,
                .srgb = request.srgb
            };
        }

        inline static Texture load(const Request& request){
            // TODO: This will be fully implemented in Phase 6-8 when RHI backends are ready
            // For now, return empty texture
            //
            // Full implementation will:
            // 1. Load image with stb_image
            // 2. Create RHI texture with device->createTexture()
            // 3. Upload pixel data with device->uploadTextureData()
            // 4. Transition to shader-readable state

            LOG_WARN(LOG_RHI, "Texture loading not yet implemented: {}", request.path.string());

            return Texture{
                .rhiTexture = {},  // Invalid handle
                .width = 0,
                .height = 0,
                .format = RHITextureFormat::Unknown,
                .mipLevels = 1
            };
        }
    };
}