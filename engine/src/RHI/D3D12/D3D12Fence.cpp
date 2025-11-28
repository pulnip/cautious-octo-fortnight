#include "D3D12Fence.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>
#include <windows.h>

namespace RenderToy
{
    bool D3D12Fence::initialize(ID3D12Device* device, uint64_t initialValue)
    {
        currentValue = initialValue;

        // Create D3D12 fence
        if (FAILED(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE,
                                       IID_PPV_ARGS(&fence)))) {
            LOG_ERROR(LOG_RHI, "Failed to create D3D12 fence");
            return false;
        }

        // Create event for CPU-GPU synchronization
        eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!eventHandle) {
            LOG_ERROR(LOG_RHI, "Failed to create fence event");
            return false;
        }

        return true;
    }

    void D3D12Fence::cleanup()
    {
        if (eventHandle) {
            CloseHandle(eventHandle);
            eventHandle = nullptr;
        }
        if (fence) {
            fence->Release();
            fence = nullptr;
        }
    }

    void D3D12Fence::signal(uint64_t value)
    {
        currentValue = value;
        // Note: Actual signaling is done via command queue
    }

    void D3D12Fence::wait(uint64_t value)
    {
        if (!fence) return;

        if (fence->GetCompletedValue() < value) {
            fence->SetEventOnCompletion(value, eventHandle);
            WaitForSingleObject(eventHandle, INFINITE);
        }
    }

    uint64_t D3D12Fence::getValue() const
    {
        return fence ? fence->GetCompletedValue() : 0;
    }

    bool D3D12Fence::isComplete(uint64_t value) const
    {
        return fence ? (fence->GetCompletedValue() >= value) : true;
    }
}

#endif // RHI_D3D12
