add_executable(RenderToyEngineTest
    Content/SceneFormatTest.cpp
    Content/MeshFormatTest.cpp
    Importer/SceneImporterTest.cpp
    Importer/MeshImporterTest.cpp
    Scene/SceneLoaderTest.cpp
    ECS/ComponentTypeRegistryTest.cpp
    ECS/EntityRegistryTest.cpp
    Resource/ResourceManagerTest.cpp
)

target_link_libraries(RenderToyEngineTest
PRIVATE
    RenderToy::Engine
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME RenderToy-Engine-Test
    COMMAND $<TARGET_FILE:RenderToyEngineTest>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(RenderToy-Engine-Test PROPERTIES LABELS "unit;core")