add_library(RenderToyImporter STATIC
    SceneImporter.cpp
)

target_include_directories(RenderToyImporter
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)

target_link_libraries(RenderToyImporter
PUBLIC
    RenderToy::Content
    tomlplusplus::tomlplusplus
)

add_library(RenderToy::Importer ALIAS RenderToyImporter)
