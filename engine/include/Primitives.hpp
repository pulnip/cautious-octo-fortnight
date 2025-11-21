#pragma once

#include "math.hpp"
#include <unordered_map>
#include "handle.hpp"
#include "string.hpp"

namespace RenderToy
{
    #define DEFINE_TRANSFORM \
        Vec3 position = zeros(); \
        Vec4 rotation = unitQuat(); \
        Vec3 scale = ones()

    enum class CameraType: uint8_t{
        UNKNOWN     = uint8_t(-1),
        MainCamera  =  0,
        SubCamera   =  1,
    };
    inline auto cameraType(const std::string& text){
        static std::unordered_map<std::string, CameraType> text2camera = {
            {"MAINCAMERA", CameraType::MainCamera},
            { "SUBCAMERA",  CameraType::SubCamera},
        };
        auto upper = toUpper(text);
        auto it = text2camera.find(upper);
        if (it == text2camera.end()){
            return CameraType::UNKNOWN;
        }
        return it->second;
    }

    enum class Projection: uint8_t{
        UNKNOWN     = uint8_t(-1),
        PERSPECTIVE =  0,
        ORTHOGRAPHIC=  1,
    };
    inline auto projection(const std::string& text){
        static std::unordered_map<std::string, Projection> text2projection = {
            {"PERSPECTIVE", Projection::PERSPECTIVE},
            {"ORTHOGRAPHIC",  Projection::ORTHOGRAPHIC},
        };
        auto upper = toUpper(text);
        auto it = text2projection.find(upper);
        if (it == text2projection.end()){
            return Projection::UNKNOWN;
        }
        return it->second;
    }

    struct Ray{
        Vec3 point;
        Vec3 dir;
    };

    struct alignas(16) Line{
        Vec4 from, to;
        Vec4 color;
    };
    struct Sphere{
        Vec3 point;
        float radius;
        Vec4 color;
    };

    using UUID = std::uint64_t;
    using MeshHandle = Handle;
    using ShaderHandle = Handle;
    using ModuleHandle = Handle;
    using ObjectHandle = Handle;
}