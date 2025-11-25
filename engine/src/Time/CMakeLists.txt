add_library(RenderToyTime STATIC
    Timer.cpp
)

target_include_directories(RenderToyTime
PUBLIC
    "${CMAKE_SOURCE_DIR}/engine/include"
)

add_library(RenderToy::Time ALIAS RenderToyTime)