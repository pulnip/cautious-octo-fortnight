add_library(RenderToyLog STATIC
    ConsoleSink.cpp
    Logger.cpp
    SDLSink.cpp
)

target_include_directories(RenderToyLog
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)
target_link_libraries(RenderToyLog
PRIVATE
    SDL3::SDL3
)

add_library(RenderToy::Log ALIAS RenderToyLog)