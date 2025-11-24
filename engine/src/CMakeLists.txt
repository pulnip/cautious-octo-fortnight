target_sources(RenderToyEngine
PRIVATE
    engine.cpp
)

add_subdirectory(ECS)
add_subdirectory(Log)
add_subdirectory(Platform)

target_link_libraries(RenderToyEngine
PUBLIC
    RenderToy::Core
PRIVATE
    RenderToy::ECS
    RenderToy::Log
    RenderToy::Platform
)
