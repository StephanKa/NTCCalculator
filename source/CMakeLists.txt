# Generic test that uses conan libs
SET(PROJECT_NAME NTCSampleCalculator)
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
                      PRIVATE project_options
                      project_warnings
                      fmt::fmt)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ../include)
