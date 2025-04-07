# Generic test that uses conan libs
FIND_PACKAGE(fmt REQUIRED)

SET(PROJECT_NAME NTCSampleCalculator)
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
                      PRIVATE project_options
                      project_warnings
                      fmt::fmt-header-only)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ../include)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/install/bin/$<TARGET_FILE_NAME:${PROJECT_NAME}>)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/include/NTCCalculator.h ${CMAKE_BINARY_DIR}/install/include/NTCCalculator.h)
