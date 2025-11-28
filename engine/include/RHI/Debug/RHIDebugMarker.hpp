#pragma once

#include "RHI/RHIPlatform.h"

namespace RenderToy
{
    // GPU debug markers for profiling and debugging
    // Shows up in RenderDoc, PIX, Xcode GPU Debugger
    // Only enabled when RHI_ENABLE_GPU_MARKERS is defined

#if RHI_ENABLE_GPU_MARKERS

    // RAII-style scoped debug event
    // Automatically ends event when going out of scope
    class ScopedDebugEvent
    {
    private:
        class RHICommandList* cmdList;

    public:
        ScopedDebugEvent(class RHICommandList* cmdList, const char* name);
        ~ScopedDebugEvent();

        // Non-copyable, non-movable
        ScopedDebugEvent(const ScopedDebugEvent&) = delete;
        ScopedDebugEvent(ScopedDebugEvent&&) = delete;
        ScopedDebugEvent& operator=(const ScopedDebugEvent&) = delete;
        ScopedDebugEvent& operator=(ScopedDebugEvent&&) = delete;
    };

    // Helper macros for easy usage
    #define RHI_DEBUG_EVENT(cmdList, name) \
        RenderToy::ScopedDebugEvent CONCAT(_debugEvent_, __LINE__)(cmdList, name)

    #define RHI_DEBUG_MARKER(cmdList, name) \
        if (cmdList) { cmdList->setMarker(name); }

#else
    // No-op in builds without GPU markers
    class ScopedDebugEvent
    {
    public:
        ScopedDebugEvent(class RHICommandList*, const char*) {}
    };

    #define RHI_DEBUG_EVENT(cmdList, name) (void)0
    #define RHI_DEBUG_MARKER(cmdList, name) (void)0

#endif

    // Helper macro for token concatenation
    #define CONCAT_IMPL(x, y) x##y
    #define CONCAT(x, y) CONCAT_IMPL(x, y)
}
