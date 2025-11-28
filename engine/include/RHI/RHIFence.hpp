#pragma once

#include "RHI/RHIResource.hpp"
#include <cstdint>

namespace RenderToy
{
    // GPU fence for CPU/GPU synchronization
    // Used to track when GPU work has completed
    class RHIFence : public RHIResource
    {
    protected:
        uint64_t currentValue;

    public:
        RHIFence(uint64_t initialValue = 0)
            : currentValue(initialValue) {}

        virtual ~RHIFence() = default;

        // Signal fence from GPU (increments to signalValue when GPU work completes)
        virtual void signal(uint64_t signalValue) = 0;

        // Wait on CPU until fence reaches waitValue
        virtual void waitCPU(uint64_t waitValue, uint64_t timeoutMs = UINT64_MAX) = 0;

        // Get current fence value (non-blocking query)
        virtual uint64_t getValue() = 0;

        // Check if fence has reached a value
        bool hasReached(uint64_t value) {
            return getValue() >= value;
        }

        // Platform-specific fence getter
        virtual void* getNativeFence() = 0;
    };
}
