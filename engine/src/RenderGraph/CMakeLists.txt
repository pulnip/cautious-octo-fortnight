add_library(RenderToyRenderGraph STATIC
    RenderGraph.cpp
    RenderGraphBuilder.cpp
    RenderGraphResources.cpp
    RenderPass.cpp
)

target_include_directories(RenderToyRenderGraph
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)

target_link_libraries(RenderToyRenderGraph
PUBLIC
    RenderToy::Core
    RenderToy::RHI
)

add_library(RenderToy::RenderGraph ALIAS RenderToyRenderGraph)
