cmake_minimum_required(VERSION 3.28)

# Enable Swift on macOS for Metal RHI backend
if(APPLE)
    project(RenderToy LANGUAGES C CXX Swift VERSION "0.0.1")
else()
    project(RenderToy LANGUAGES C CXX VERSION "0.0.1")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RENDERTOY_ENABLE_TEST "Enable Tests" ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(FetchContent)

find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.2.26"
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# stb - header-only image loading library
FetchContent_Declare(
    stb
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG "master"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(stb)

include(cmake/sanitizer.cmake)

if(RENDERTOY_ENABLE_TEST)
    find_package(GTest QUIET)
    if(NOT gtest_FOUND)
        FetchContent_Declare(
            GTest
            GIT_REPOSITORY "https://github.com/google/googletest.git"
            GIT_TAG "v1.17.0"
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable(GTest)
    endif()
    include(CTest)
    enable_testing()
endif()

add_subdirectory(Core)
add_subdirectory(engine)
add_subdirectory(samples)

