add_library(RenderToyContent STATIC
    MeshFormat.cpp
)

target_include_directories(RenderToyContent
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)

target_link_libraries(RenderToyContent
PUBLIC
    RenderToy::Core
)

add_library(RenderToy::Content ALIAS RenderToyContent)
