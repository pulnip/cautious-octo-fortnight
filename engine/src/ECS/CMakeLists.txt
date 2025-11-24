add_library(RenderToyECS STATIC
    AnimationSystem.cpp
    EntityRegistry.cpp
    PhysicsSystem.cpp
    RenderSystem.cpp
    TransformSystem.cpp
    UISystem.cpp
    World.cpp
)

target_include_directories(RenderToyECS
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)
target_link_libraries(RenderToyECS
PUBLIC
    RenderToy::Core
)

add_library(RenderToy::ECS ALIAS RenderToyECS)