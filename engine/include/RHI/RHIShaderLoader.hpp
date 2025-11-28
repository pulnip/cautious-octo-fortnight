#pragma once

#include "RHIDefinitions.hpp"
#include "RHITypes.hpp"
#include "RHIDevice.hpp"
#include "Log/Category.hpp"
#include "Log/Log.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <optional>

namespace RenderToy
{
    // Helper functions for loading shader bytecode
    namespace RHIShaderLoader
    {
        // Load shader bytecode from file
        inline std::optional<std::vector<uint8_t>> loadShaderBytecode(
            const std::filesystem::path& path)
        {
            if (!std::filesystem::exists(path)) {
                LOG_ERROR(LOG_RHI, "Shader file not found: {}", path.string());
                return std::nullopt;
            }

            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                LOG_ERROR(LOG_RHI, "Failed to open shader file: {}", path.string());
                return std::nullopt;
            }

            const size_t fileSize = file.tellg();
            if (fileSize == 0) {
                LOG_ERROR(LOG_RHI, "Shader file is empty: {}", path.string());
                return std::nullopt;
            }

            std::vector<uint8_t> bytecode(fileSize);
            file.seekg(0);
            file.read(reinterpret_cast<char*>(bytecode.data()), fileSize);

            if (!file) {
                LOG_ERROR(LOG_RHI, "Failed to read shader file: {}", path.string());
                return std::nullopt;
            }

            LOG_INFO(LOG_RHI, "Loaded shader bytecode: {} ({} bytes)",
                     path.filename().string(), fileSize);
            return bytecode;
        }

        // Create shader from bytecode file
        inline RHIShaderHandle createShaderFromFile(
            RHIDevice* device,
            const std::filesystem::path& path,
            RHIShaderStage stage,
            const char* entryPoint = "main",
            const char* debugName = nullptr)
        {
            auto bytecode = loadShaderBytecode(path);
            if (!bytecode) {
                return RHIShaderHandle{};  // Invalid handle
            }

            RHIShaderCreateDesc desc{
                .stage = stage,
                .bytecode = bytecode->data(),
                .bytecodeSize = bytecode->size(),
                .entryPoint = entryPoint,
                .debugName = debugName ? debugName :
                    path.filename().string().c_str()
            };

            return device->createShader(desc);
        }

        // Load vertex shader from file
        inline RHIShaderHandle loadVertexShader(
            RHIDevice* device,
            const std::filesystem::path& path,
            const char* entryPoint = "main",
            const char* debugName = nullptr)
        {
            return createShaderFromFile(device, path, RHIShaderStage::Vertex,
                                       entryPoint, debugName);
        }

        // Load pixel/fragment shader from file
        inline RHIShaderHandle loadPixelShader(
            RHIDevice* device,
            const std::filesystem::path& path,
            const char* entryPoint = "main",
            const char* debugName = nullptr)
        {
            return createShaderFromFile(device, path, RHIShaderStage::Pixel,
                                       entryPoint, debugName);
        }

        // Load compute shader from file
        inline RHIShaderHandle loadComputeShader(
            RHIDevice* device,
            const std::filesystem::path& path,
            const char* entryPoint = "main",
            const char* debugName = nullptr)
        {
            return createShaderFromFile(device, path, RHIShaderStage::Compute,
                                       entryPoint, debugName);
        }

        // Helper to get platform-specific shader extension
        inline const char* getShaderExtension()
        {
#if RHI_D3D12
            return ".dxil";  // DirectX 12 compiled shader
#elif RHI_METAL
            return ".metallib";  // Metal library
#else
            return ".bin";  // Generic binary
#endif
        }

        // Helper to construct platform-specific shader path
        // Example: "shaders/basic" -> "shaders/basic.dxil" on Windows
        //                          -> "shaders/basic.metallib" on macOS
        inline std::filesystem::path getPlatformShaderPath(
            const std::filesystem::path& basePath)
        {
            std::filesystem::path result = basePath;
            result += getShaderExtension();
            return result;
        }

        // Load shader with automatic platform extension
        inline RHIShaderHandle loadShaderAuto(
            RHIDevice* device,
            const std::filesystem::path& basePathWithoutExtension,
            RHIShaderStage stage,
            const char* entryPoint = "main",
            const char* debugName = nullptr)
        {
            auto platformPath = getPlatformShaderPath(basePathWithoutExtension);
            return createShaderFromFile(device, platformPath, stage,
                                       entryPoint, debugName);
        }

        // Validate shader bytecode format (basic check)
        inline bool validateShaderBytecode(const std::vector<uint8_t>& bytecode)
        {
            if (bytecode.empty()) {
                return false;
            }

#if RHI_D3D12
            // DXIL bytecode starts with "DXBC" (DirectX Bytecode Container)
            if (bytecode.size() >= 4) {
                return bytecode[0] == 'D' && bytecode[1] == 'X' &&
                       bytecode[2] == 'B' && bytecode[3] == 'C';
            }
#elif RHI_METAL
            // Metal library format validation
            // TODO: Add Metal library format validation when needed
            // For now, just check it's not empty
            return true;
#endif
            return false;
        }
    }
}
