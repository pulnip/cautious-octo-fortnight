#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>
#include "Primitives.hpp"

namespace RenderToy
{
    // ============================================================================
    // Vertex Format
    // ============================================================================

    // Standard vertex format
    // Matches common 3D model formats (glTF, FBX, OBJ)
    struct Vertex{
        Vec3 position;
        Vec3 normal;
        Vec2 texCoord;
        Vec4 tangent;  // xyz = tangent direction, w = handedness sign

        // Optional: vertex colors, bone weights, etc. can be added later
    };
    static_assert(sizeof(Vertex) == 48, "Vertex should be 48 bytes");
    static_assert(std::is_trivially_copyable_v<Vertex>, "Vertex must be trivially copyable");

    // ============================================================================
    // Coordinate System
    // ============================================================================

    enum class Handedness: uint8_t{
        RightHanded = 0,
        LeftHanded = 1
    };

    // Axis information for imported meshes
    // Different 3D tools use different conventions
    struct AxisInfo{
        Handedness handedness = Handedness::LeftHanded;
        char upAxis = 'Y';        // Which axis points up ('X', 'Y', or 'Z')
        char forwardAxis = 'Z';   // Which axis points forward
        bool flipTexCoordV = true; // Flip V coordinate (OpenGL vs DirectX)
        double unitScale = 0.01f;   // Conversion factor (e.g., 0.01 for cm to meters)
    };

    // ============================================================================
    // Bounding Volume
    // ============================================================================

    /// Axis-Aligned Bounding Box
    struct AABB{
        Vec3 min = zeros();
        Vec3 max = zeros();

        inline Vec3 center() const { return (min + max) * 0.5f; }
        inline Vec3 extents() const { return (max - min) * 0.5f; }
        inline bool isValid() const { return min.x <= max.x && min.y <= max.y && min.z <= max.z; }
    };

    // ============================================================================
    // Primitive Type
    // ============================================================================

    // GPU primitive topology
    enum class PrimitiveType: uint8_t{
        PointList = 0,
        LineList = 1,
        LineStrip = 2,
        TriangleList = 3,
        TriangleStrip = 4
    };

    // ============================================================================
    // Material System
    // ============================================================================

    // Material type (shading model)
    enum class MaterialType: uint8_t{
        Unlit = 0,  // No lighting, just texture/color
        PBR = 1     // Physically-Based Rendering
    };

    // Texture usage semantic
    enum class TextureUsage: uint8_t{
        BaseColor = 0,  // Albedo / Diffuse
        Normal = 1,     // Normal map (tangent space)
        MetallicRoughness = 2,  // R=unused, G=Roughness, B=Metallic (glTF 2.0 convention)
        Emissive = 3,   // Emission map
        Occlusion = 4,  // Ambient occlusion

        // Future: add more as needed
        // Height, Opacity, etc.
    };

    /// Texture flags
    enum TextureFlags: uint16_t{
        TextureFlag_None = 0,
        TextureFlag_SRGB = 1 << 0,  // Texture is in sRGB color space
        TextureFlag_GenerateMips = 1 << 1  // Generate mipmaps
    };

    /// Texture reference in a material
    struct TextureDescriptor{
        std::string uri;  // Path to texture file
        TextureUsage usage;
        uint16_t flags = TextureFlag_None;
    };

    // Describes how a surface should be shaded
    struct MaterialDescriptor{
        std::string name;
        MaterialType type = MaterialType::PBR;

        // Textures keyed by usage
        std::unordered_map<TextureUsage, TextureDescriptor> textures;

        // Material parameters (when textures are not present)
        Vec4 baseColorFactor = Vec4{1.0f, 1.0f, 1.0f, 1.0f};
        double metallicFactor = 0.0f;
        double roughnessFactor = 1.0f;
        Vec3 emissiveFactor = zeros();

        // Check if material has a specific texture
        inline bool hasTexture(TextureUsage usage) const {
            return textures.find(usage) != textures.end();
        }
    };

    // ============================================================================
    // Submesh
    // ============================================================================

    // Submesh (mesh part with single material)
    // A mesh can contain multiple submeshes with different materials
    struct SubmeshDescriptor{
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;

        std::string materialSlotName;  // Which material to use

        inline uint32_t vertexCount() const { return static_cast<uint32_t>(vertices.size()); }
        inline uint32_t indexCount() const { return static_cast<uint32_t>(indices.size()); }
        inline uint32_t triangleCount() const {
            return primitiveType == PrimitiveType::TriangleList ? indexCount() / 3 : 0;
        }
    };

    // ============================================================================
    // Mesh Data
    // ============================================================================

    // Runtime mesh representation
    // Contains geometry data and material information
    struct MeshData{ // previously CookedMesh
        // Metadata
        AxisInfo axisInfo;
        AABB bounds;

        // Geometry
        std::vector<SubmeshDescriptor> submeshes;

        // Materials (keyed by slot name)
        std::unordered_map<std::string, MaterialDescriptor> materials;

        inline uint32_t submeshCount() const{ return static_cast<uint32_t>(submeshes.size()); }
        inline uint32_t materialCount() const{ return static_cast<uint32_t>(materials.size()); }

        inline uint32_t totalVertexCount() const{
            uint32_t count = 0;
            for (const auto& submesh : submeshes){
                count += submesh.vertexCount();
            }
            return count;
        }

        inline uint32_t totalIndexCount() const{
            uint32_t count = 0;
            for (const auto& submesh: submeshes){
                count += submesh.indexCount();
            }
            return count;
        }

        inline bool isValid() const{
            return !submeshes.empty() && bounds.isValid();
        }

        // Find material by slot name
        inline const MaterialDescriptor* findMaterial(const std::string& slotName) const {
            auto it = materials.find(slotName);
            return it != materials.end() ? &it->second : nullptr;
        }
    };

    // ============================================================================
    // Binary Serialization (File Format)
    // ============================================================================

    /// Magic number for RenderToy mesh files
    constexpr char MESH_FILE_MAGIC[8] = "RTMESH\x01";
    constexpr uint32_t MESH_FILE_VERSION = 1;

    // Binary format for efficient loading
    struct MeshFileHeader {
        char magic[8] = {'R', 'T', 'M', 'E', 'S', 'H', '\x01', '\0'};
        uint32_t version = MESH_FILE_VERSION;
        uint32_t headerSize = sizeof(MeshFileHeader);

        // Section offsets (from file start)
        uint32_t submeshTableOffset = 0;
        uint32_t vertexDataOffset = 0;
        uint32_t indexDataOffset = 0;
        uint32_t materialTableOffset = 0;
        uint32_t stringBlobOffset = 0;

        // Section sizes
        uint32_t submeshCount = 0;
        uint32_t totalVertexCount = 0;
        uint32_t totalIndexCount = 0;
        uint32_t materialCount = 0;
        uint32_t stringBlobSize = 0;

        // Metadata
        AxisInfo axisInfo;
        AABB bounds;
    };
    static_assert(sizeof(MeshFileHeader) % 16 == 0, "Header should be 16-byte aligned");

    // ============================================================================
    // Serialization Functions
    // ============================================================================

    // Serialize mesh to binary format
    // Returns byte buffer suitable for writing to disk
    std::vector<uint8_t> serializeMesh(const MeshData& mesh);

    // Deserialize mesh from binary format
    // Returns std::nullopt if data is invalid
    std::optional<MeshData> deserializeMesh(std::span<const uint8_t> data);
}
