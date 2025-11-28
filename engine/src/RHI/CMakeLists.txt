# RHI (Render Hardware Interface) library

add_library(RenderToyRHI STATIC)

# Common RHI sources (platform-agnostic)
set(RHI_COMMON_SOURCES
    RHIDevice.cpp
    Debug/RHIDebugMarker.cpp
)

target_sources(RenderToyRHI PRIVATE ${RHI_COMMON_SOURCES})

# Platform-specific RHI sources and libraries
if(WIN32)
    # DirectX 12 backend
    set(RHI_PLATFORM_SOURCES
        D3D12/D3D12Device.cpp
        D3D12/D3D12CommandList.cpp
        D3D12/D3D12Buffer.cpp
        D3D12/D3D12Texture.cpp
        D3D12/D3D12Shader.cpp
        D3D12/D3D12PipelineState.cpp
        D3D12/D3D12Swapchain.cpp
        D3D12/D3D12Fence.cpp
    )

    set(RHI_PLATFORM_LIBS
        d3d12.lib
        dxgi.lib
        dxguid.lib
    )

    target_compile_definitions(RenderToyRHI PUBLIC RHI_D3D12=1)

elseif(APPLE)
    # Metal backend - Swift implementation with C++ wrappers

    # C++ wrapper sources
    set(RHI_PLATFORM_SOURCES
        Metal/MetalDeviceFactory.cpp
        Metal/MetalDevice_CPPWrapper.cpp
        Metal/MetalCommandList_CPPWrapper.cpp
    )

    # Swift implementation 
    add_library(RenderToyMetalRHI STATIC)

    set(RHI_SWIFT_SOURCES
        Metal/MetalDevice.swift
        Metal/MetalCommandList.swift
    )

    # Add Swift sources to target
    target_sources(RenderToyMetalRHI PRIVATE ${RHI_SWIFT_SOURCES})

    target_compile_options(RenderToyMetalRHI
    PRIVATE
        "-import-objc-header" "${CMAKE_CURRENT_SOURCE_DIR}/Metal/RenderToyMetalRHI-Bridging-Header.h"
    )

    target_include_directories(RenderToyMetalRHI
    PUBLIC
        ${CMAKE_SOURCE_DIR}/engine/include
    )

    # Configure Swift compilation
    set_target_properties(RenderToyMetalRHI PROPERTIES
        Swift_MODULE_NAME "RenderToyMetalRHI"
        Swift_MODULE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/swiftmodules"
    )

    # Find Metal frameworks
    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(METALKIT_LIBRARY MetalKit REQUIRED)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)
    find_library(FOUNDATION_LIBRARY Foundation REQUIRED)

    set(RHI_PLATFORM_LIBS
        RenderToyMetalRHI
    )

    target_link_libraries(RenderToyMetalRHI
    PRIVATE
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${FOUNDATION_LIBRARY}
    )

    target_compile_definitions(RenderToyRHI PUBLIC RHI_METAL=1)
else()
    message(FATAL_ERROR "Unsupported platform for RHI. Only Windows and macOS are supported.")
endif()

# Add platform sources if they exist
if(RHI_PLATFORM_SOURCES)
    target_sources(RenderToyRHI PRIVATE ${RHI_PLATFORM_SOURCES})
endif()

# Link platform libraries
if(RHI_PLATFORM_LIBS)
    target_link_libraries(RenderToyRHI PRIVATE ${RHI_PLATFORM_LIBS})
endif()

# Include directories
target_include_directories(RenderToyRHI
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)

# Link dependencies
target_link_libraries(RenderToyRHI
PUBLIC
    RenderToy::Core
    SDL3::SDL3
)

# Create alias
add_library(RenderToy::RHI ALIAS RenderToyRHI)

message(STATUS "RHI backend: ${RHI_PLATFORM_LIBS}")
