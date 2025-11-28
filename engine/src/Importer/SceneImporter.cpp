#include "Importer/SceneImporter.hpp"
#include <toml++/toml.hpp>
#include <format>
#include <fstream>

using namespace RenderToy;

namespace
{
    // ========================================================================
    // Helper Functions
    // ========================================================================

    template<typename T>
    std::optional<T> readOptional(const toml::table& table, const char* key){
        if(auto val = table[key]){
            if constexpr(std::is_same_v<T, std::string>){
                if(auto s = val.as_string())
                    return std::string{s->get()};
            } else if constexpr(std::is_same_v<T, float>){
                if(auto f = val.as_floating_point())
                    return static_cast<float>(f->get());
                if(auto i = val.as_integer())
                    return static_cast<float>(i->get());
            } else if constexpr(std::is_same_v<T, bool>){
                if(auto b = val.as_boolean())
                    return b->get();
            }
        }
        return std::nullopt;
    }

    template<unsigned N>
    std::optional<std::conditional_t<N==3, Vec3, Vec4>> readVec(
        const toml::table& table, const char* key
    ){
        if(auto val = table[key]){
            if(auto arr = val.as_array()){
                if(arr->size() != N)
                    return std::nullopt;

                std::conditional_t<N==3, Vec3, Vec4> result;
                for(size_t i = 0; i < N; ++i){
                    if(auto f = (*arr)[i].as_floating_point())
                        result[i] = static_cast<float>(f->get());
                    else if(auto num = (*arr)[i].as_integer())
                        result[i] = static_cast<float>(num->get());
                    else
                        return std::nullopt;
                }
                return result;
            }
        }
        return std::nullopt;
    }

    // ========================================================================
    // Component Parsers
    // ========================================================================

    std::optional<TransformDescriptor> parseTransform(const toml::table& table){
        TransformDescriptor desc;

        // Check for invalid vec lengths (if field exists, it must be valid)
        if(table.contains("position")){
            auto pos = readVec<3>(table, "position");
            if(!pos) return std::nullopt;  // Invalid length
            desc.position = *pos;
        } else {
            desc.position = zeros();
        }

        if(table.contains("rotation")){
            auto rot = readVec<4>(table, "rotation");
            if(!rot) return std::nullopt;
            desc.rotation = *rot;
        } else {
            desc.rotation = unitQuat();
        }

        if(table.contains("scale")){
            auto scl = readVec<3>(table, "scale");
            if(!scl) return std::nullopt;
            desc.scale = *scl;
        } else {
            desc.scale = ones();
        }

        return desc;
    }

    PhysicsMaterialDescriptor parseColliderMaterial(const toml::table& table){
        PhysicsMaterialDescriptor desc;

        if(auto matTable = table["material"].as_table()){
            desc.friction = readOptional<float>(*matTable, "friction").value_or(0.5f);
            desc.bounciness = readOptional<float>(*matTable, "bounciness").value_or(0.0f);
        }

        return desc;
    }

    ShaderDescriptor parseShader(const toml::table& table){
        ShaderDescriptor desc;

        // Set defaults first
        desc.modulePath = "";
        desc.vertexFunction = "vs_main";
        desc.fragmentFunction = "fs_main";

        // Override with values from table if present
        if(auto shaderTable = table["shader"].as_table()){
            desc.modulePath = readOptional<std::string>(*shaderTable, "modulePath").value_or("");
            desc.vertexFunction = readOptional<std::string>(*shaderTable, "vertexFunction").value_or("vs_main");
            desc.fragmentFunction = readOptional<std::string>(*shaderTable, "fragmentFunction").value_or("fs_main");
        }

        return desc;
    }

    std::optional<MeshDescriptor> parseMesh(const toml::table& table){
        MeshDescriptor desc;

        auto uri = readOptional<std::string>(table, "uri");
        if(!uri)
            return std::nullopt;

        desc.uri = *uri;

        // Parse material overrides
        if(auto matArray = table["materialOverride"].as_array()){
            for(const auto& elem : *matArray){
                if(auto matTable = elem.as_table()){
                    MaterialOverride mat;

                    auto baseColor = readOptional<std::string>(*matTable, "baseColorTexture");
                    auto slot = readOptional<std::string>(*matTable, "targetSlot");

                    if(baseColor && slot){
                        mat.baseColorTexture = *baseColor;
                        mat.targetSlot = *slot;
                        desc.materialOverrides.push_back(mat);
                    }
                }
            }
        }

        // Parse shader
        desc.shader = parseShader(table);

        return desc;
    }

    std::optional<RigidbodyDescriptor> parseRigidbody(const toml::table& table){
        RigidbodyDescriptor desc;

        desc.velocity = readVec<3>(table, "velocity").value_or(zeros());
        desc.useGravity = readOptional<bool>(table, "useGravity").value_or(true);
        desc.mass = readOptional<float>(table, "mass").value_or(1.0f);

        return desc;
    }

    std::optional<BoxColliderDescriptor> parseBoxCollider(const toml::table& table){
        BoxColliderDescriptor desc;

        desc.center = readVec<3>(table, "center").value_or(zeros());
        desc.size = readVec<3>(table, "size").value_or(ones());
        desc.material = parseColliderMaterial(table);

        return desc;
    }

    std::optional<SphereColliderDescriptor> parseSphereCollider(const toml::table& table){
        SphereColliderDescriptor desc;

        desc.center = readVec<3>(table, "center").value_or(zeros());

        auto radius = readOptional<float>(table, "radius");
        if(!radius)
            return std::nullopt;

        desc.radius = *radius;
        desc.material = parseColliderMaterial(table);

        return desc;
    }

    std::optional<CameraDescriptor> parseCamera(const toml::table& table){
        CameraDescriptor desc;

        auto fov = readOptional<float>(table, "fov");
        if(!fov)
            return std::nullopt;

        desc.fov = *fov;
        desc.nearPlane = readOptional<float>(table, "nearPlane").value_or(0.1f);
        desc.farPlane = readOptional<float>(table, "farPlane").value_or(1000.0f);

        // Parse projection type (optional, defaults to Perspective)
        if(auto projStr = readOptional<std::string>(table, "projection")){
            if(*projStr == "Orthographic" || *projStr == "orthographic")
                desc.projection = ProjectionType::Orthographic;
            else
                desc.projection = ProjectionType::Perspective;
        }

        // Parse orthographic parameters if needed
        desc.orthoWidth = readOptional<float>(table, "orthoWidth").value_or(10.0f);
        desc.orthoHeight = readOptional<float>(table, "orthoHeight").value_or(10.0f);

        return desc;
    }

    // ========================================================================
    // Entity Parser
    // ========================================================================

    std::optional<EntityDescriptor> parseEntity(const toml::table& entityTable){
        EntityDescriptor entity;

        // Parse name
        if(auto name = entityTable["name"].as_string())
            entity.name = std::string{name->get()};
        else
            entity.name = "Unnamed";

        // Parse components (each component is a nested table)
        // If a component table exists, it must parse successfully
        if(auto transformTable = entityTable["transform"].as_table()){
            auto result = parseTransform(*transformTable);
            if(!result) return std::nullopt;  // Failed to parse - abort entity
            entity.transform = *result;
        }

        if(auto meshTable = entityTable["mesh"].as_table()){
            auto result = parseMesh(*meshTable);
            if(!result) return std::nullopt;
            entity.mesh = *result;
        }

        if(auto rigidbodyTable = entityTable["rigidbody"].as_table()){
            auto result = parseRigidbody(*rigidbodyTable);
            if(!result) return std::nullopt;
            entity.rigidbody = *result;
        }

        if(auto boxColliderTable = entityTable["boxCollider"].as_table()){
            auto result = parseBoxCollider(*boxColliderTable);
            if(!result) return std::nullopt;
            entity.boxCollider = *result;
        }

        if(auto sphereColliderTable = entityTable["sphereCollider"].as_table()){
            auto result = parseSphereCollider(*sphereColliderTable);
            if(!result) return std::nullopt;
            entity.sphereCollider = *result;
        }

        if(auto cameraTable = entityTable["camera"].as_table()){
            auto result = parseCamera(*cameraTable);
            if(!result) return std::nullopt;
            entity.camera = *result;
        }

        return entity;
    }

    // ========================================================================
    // Scene Parser
    // ========================================================================

    std::optional<SceneDescriptor> parseSceneFromTable(const toml::table& root){
        SceneDescriptor scene;

        // Parse entities array
        if(auto entitiesArray = root["entities"].as_array()){
            for(const auto& elem : *entitiesArray){
                if(auto entityTable = elem.as_table()){
                    auto entity = parseEntity(*entityTable);
                    if(!entity)
                        return std::nullopt;  // Failed to parse entity - abort entire scene
                    scene.entities.push_back(*entity);
                }
            }
        }

        return scene;
    }
}

namespace RenderToy
{
    std::optional<SceneDescriptor> importScene(const std::string& filePath){
        try{
            auto result = toml::parse_file(filePath);
            return parseSceneFromTable(result);
        }
        catch(const toml::parse_error& e){
            // Log error if needed
            return std::nullopt;
        }
        catch(...){
            return std::nullopt;
        }
    }

    std::optional<SceneDescriptor> importSceneFromString(std::string_view tomlText){
        try{
            auto result = toml::parse(tomlText);
            return parseSceneFromTable(result);
        }
        catch(const toml::parse_error& e){
            // Log error if needed
            return std::nullopt;
        }
        catch(...){
            return std::nullopt;
        }
    }
}
