add_library(RenderToyPlatform STATIC
    Window.cpp
)

target_include_directories(RenderToyPlatform
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)
target_link_libraries(RenderToyPlatform
PRIVATE
    SDL3::SDL3
)

add_library(RenderToy::Platform ALIAS RenderToyPlatform)