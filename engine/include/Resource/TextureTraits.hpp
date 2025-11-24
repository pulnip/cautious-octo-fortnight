#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include "Resource/ResourceTraits.hpp"
#include "Resource/Texture.hpp"

namespace RenderToy
{
    enum class TextureFormat{
        R8G8B8A8_UNORM,
    };

    enum class SamplingMode{
        Nearest,
        Linear,
    };

    struct TextureRequest{
        std::filesystem::path path;
        TextureFormat format = TextureFormat::R8G8B8A8_UNORM;
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
        inline std::size_t operator()(const TextureKey& k) const noexcept{
            std::size_t hash = std::hash<std::string>{}(k.canonicalPath);
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
            // TODO

            return Texture{};
        }
    };
}