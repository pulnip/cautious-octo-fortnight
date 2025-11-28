#pragma once

#include "RHI/RHITypes.hpp"
#include "RHI/RHIDefinitions.hpp"
#include <cstdint>

// Forward declarations for D3D12/DXGI
struct IDXGISwapChain3;
struct ID3D12Device;
struct ID3D12Resource;

namespace RenderToy
{
    struct D3D12Swapchain
    {
        IDXGISwapChain3* swapchain = nullptr;
        ID3D12Resource** backbuffers = nullptr;
        uint32_t bufferCount = 0;
        uint32_t currentFrameIndex = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        bool vsyncEnabled = true;

        bool initialize(ID3D12Device* device,
                       void* windowHandle,
                       uint32_t width,
                       uint32_t height,
                       uint32_t bufferCount,
                       bool vsync);

        void cleanup();
        void resize(uint32_t newWidth, uint32_t newHeight);
        void acquireNextImage();
        ID3D12Resource* getCurrentBackbuffer() const;
    };
}
