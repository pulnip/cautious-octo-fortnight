#include "D3D12Buffer.hpp"
#include "Log/Log.hpp"
#include "Log/Category.hpp"

#if RHI_D3D12

#include <d3d12.h>
#include <cstring>

namespace RenderToy
{
    bool D3D12Buffer::initialize(ID3D12Device* device, const RHIBufferCreateDesc& desc)
    {
        size = desc.size;
        usage = desc.usage;

        // Create buffer resource
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;  // TODO: Choose appropriate heap type

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = desc.size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buffer)
        );

        if (FAILED(hr)) {
            LOG_ERROR(LOG_RHI, "Failed to create D3D12 buffer");
            return false;
        }

        // Upload initial data if provided
        if (desc.initialData) {
            upload(desc.initialData, desc.size, 0);
        }

        return true;
    }

    void D3D12Buffer::cleanup()
    {
        if (buffer) {
            buffer->Release();
            buffer = nullptr;
        }
    }

    void D3D12Buffer::upload(const void* data, size_t dataSize, size_t offset)
    {
        if (!buffer || !data) return;

        // Map, copy, unmap
        void* mappedData = nullptr;
        D3D12_RANGE readRange = { 0, 0 };  // We're not reading
        if (SUCCEEDED(buffer->Map(0, &readRange, &mappedData))) {
            memcpy(static_cast<uint8_t*>(mappedData) + offset, data, dataSize);
            buffer->Unmap(0, nullptr);
        }
    }
}

#endif // RHI_D3D12
