#pragma once

// Platform detection macros for RHI backend selection
#if defined(_WIN32) || defined(_WIN64)
    #define RHI_PLATFORM_WINDOWS 1
    #define RHI_PLATFORM_MACOS 0
    #ifndef RHI_D3D12
        #define RHI_D3D12 1
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define RHI_PLATFORM_MACOS 1
        #define RHI_PLATFORM_WINDOWS 0
        #ifndef RHI_METAL
            #define RHI_METAL 1
        #endif
    #else
        #error "Unsupported Apple platform"
    #endif
#else
    #error "Unsupported platform. Only Windows and macOS are supported."
#endif

// Validation and debug layer control
#ifndef RHI_ENABLE_VALIDATION
    #if defined(_DEBUG) || defined(DEBUG)
        #define RHI_ENABLE_VALIDATION 1
    #else
        #define RHI_ENABLE_VALIDATION 0
    #endif
#endif

// Resource tracking for leak detection
#ifndef RHI_ENABLE_RESOURCE_TRACKING
    #if defined(_DEBUG) || defined(DEBUG)
        #define RHI_ENABLE_RESOURCE_TRACKING 1
    #else
        #define RHI_ENABLE_RESOURCE_TRACKING 0
    #endif
#endif

// GPU profiling markers
#ifndef RHI_ENABLE_GPU_MARKERS
    #define RHI_ENABLE_GPU_MARKERS 1
#endif
