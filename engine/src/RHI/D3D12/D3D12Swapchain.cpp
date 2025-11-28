#include "D3D12Swapchain.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>
#include <dxgi1_4.h>

namespace RenderToy
{
    bool D3D12Swapchain::initialize(ID3D12Device* device,
                                   void* windowHandle,
                                   uint32_t width,
                                   uint32_t height,
                                   uint32_t bufferCount,
                                   bool vsync)
    {
        this->width = width;
        this->height = height;
        this->bufferCount = bufferCount;
        this->vsyncEnabled = vsync;

        // TODO: Create swapchain
        LOG_WARN(LOG_RHI, "D3D12Swapchain::initialize not yet implemented");
        return false;
    }

    void D3D12Swapchain::cleanup()
    {
        if (backbuffers) {
            for (uint32_t i = 0; i < bufferCount; ++i) {
                if (backbuffers[i]) {
                    backbuffers[i]->Release();
                }
            }
            delete[] backbuffers;
            backbuffers = nullptr;
        }

        if (swapchain) {
            swapchain->Release();
            swapchain = nullptr;
        }
    }

    void D3D12Swapchain::resize(uint32_t newWidth, uint32_t newHeight)
    {
        // TODO: Implement resize
        LOG_WARN(LOG_RHI, "D3D12Swapchain::resize not yet implemented");
    }

    void D3D12Swapchain::acquireNextImage()
    {
        if (swapchain) {
            currentFrameIndex = swapchain->GetCurrentBackBufferIndex();
        }
    }

    ID3D12Resource* D3D12Swapchain::getCurrentBackbuffer() const
    {
        if (backbuffers && currentFrameIndex < bufferCount) {
            return backbuffers[currentFrameIndex];
        }
        return nullptr;
    }
}

#endif // RHI_D3D12
