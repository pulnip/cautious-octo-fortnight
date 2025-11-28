add_library(RenderToyEngine STATIC)

target_include_directories(RenderToyEngine
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
PRIVATE
    "${stb_SOURCE_DIR}"
)

add_subdirectory(src)

if(WIN32)
    set_property(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        PROPERTY VS_STARTUP_PROJECT RenderToyEngine
    )
elseif(APPLE)
    target_link_libraries(RenderToyEngine
    PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

add_library(RenderToy::Engine ALIAS RenderToyEngine)

if(RENDERTOY_ENABLE_TEST)
    add_subdirectory(test)
endif()
