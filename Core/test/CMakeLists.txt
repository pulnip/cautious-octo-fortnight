add_executable(RenderToyCoreTest
    dynamic_vector.cpp
    math_test.cpp
)

target_link_libraries(RenderToyCoreTest
PRIVATE
    RenderToy::Core
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME RenderToy-Core-Test
    COMMAND $<TARGET_FILE:RenderToyCoreTest>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(RenderToy-Core-Test PROPERTIES LABELS "unit;core")