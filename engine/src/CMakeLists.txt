target_sources(RenderToyEngine
PRIVATE
    engine.cpp
)

add_subdirectory(ECS)
add_subdirectory(Log)

target_link_libraries(RenderToyEngine
PRIVATE
    RenderToy::ECS
    RenderToy::Log
)
