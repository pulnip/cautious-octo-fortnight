add_library(RenderToyScene STATIC
    SceneLoader.cpp
)

target_include_directories(RenderToyScene
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)

target_link_libraries(RenderToyScene
PUBLIC
    RenderToy::Content
    RenderToy::ECS
    RenderToy::Log
)

add_library(RenderToy::Scene ALIAS RenderToyScene)
