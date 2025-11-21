if(WIN32)
    add_executable(RenderToyDemo WIN32)
elseif(APPLE)
    add_executable(RenderToyDemo MACOSX_BUNDLE)
else()
    add_executable(RenderToyDemo)
endif()

target_include_directories(RenderToyDemo
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(RenderToyDemo
PRIVATE
    RenderToyEngine
    SDL3::SDL3
)

add_subdirectory(src)