ADD_EXECUTABLE(tests tests.cpp)
TARGET_LINK_LIBRARIES(tests PRIVATE project_warnings project_options fmt::fmt Catch2::Catch2WithMain)
TARGET_INCLUDE_DIRECTORIES(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

INCLUDE(CTest)
INCLUDE(Catch)
# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
CATCH_DISCOVER_TESTS(
        tests
        TEST_PREFIX
        "unittests."
        REPORTER
        JUnit
        OUTPUT_DIR
        .
        OUTPUT_PREFIX
        "unittests."
        OUTPUT_SUFFIX
        .xml)

ADD_CUSTOM_COMMAND(TARGET tests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tests> ${CMAKE_BINARY_DIR}/install/bin/$<TARGET_FILE_NAME:tests>)
