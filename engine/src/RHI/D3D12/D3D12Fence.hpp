#pragma once

#include <cstdint>

// Forward declarations for D3D12
struct ID3D12Fence;
struct ID3D12Device;

namespace RenderToy
{
    struct D3D12Fence
    {
        ID3D12Fence* fence = nullptr;
        void* eventHandle = nullptr;  // HANDLE on Windows
        uint64_t currentValue = 0;

        bool initialize(ID3D12Device* device, uint64_t initialValue);
        void cleanup();

        void signal(uint64_t value);
        void wait(uint64_t value);
        uint64_t getValue() const;
        bool isComplete(uint64_t value) const;
    };
}
