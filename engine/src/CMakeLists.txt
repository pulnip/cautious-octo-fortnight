target_sources(RenderToyEngine
PRIVATE
    Engine.cpp
)

add_subdirectory(ECS)
add_subdirectory(Log)
add_subdirectory(Platform)
add_subdirectory(Time)
add_subdirectory(RHI)
add_subdirectory(RenderGraph)

target_link_libraries(RenderToyEngine
PUBLIC
    RenderToy::Core
PRIVATE
    RenderToy::ECS
    RenderToy::Log
    RenderToy::Platform
    RenderToy::Time
    RenderToy::RHI
    RenderToy::RenderGraph
)
