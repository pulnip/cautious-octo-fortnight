add_library(RenderToyECS STATIC
    EntityRegistry.cpp
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