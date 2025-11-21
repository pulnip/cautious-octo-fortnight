add_library(RenderToyCore INTERFACE)

target_include_directories(RenderToyCore
INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(RenderToy::Core ALIAS RenderToyCore)

if(RENDERTOY_ENABLE_TEST)
    add_subdirectory(test)
endif()