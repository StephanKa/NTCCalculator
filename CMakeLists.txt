CMAKE_MINIMUM_REQUIRED(VERSION 3.19)
# Download automatically, you can also just copy the conan.cmake file
IF(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    MESSAGE(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    FILE(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop2/conan_provider.cmake" "${CMAKE_BINARY_DIR}/conan.cmake" TLS_VERIFY ON)
ENDIF()
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
SET(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_BINARY_DIR}/conan.cmake")

# Set the project name to your project name, my project isn't very descriptive
PROJECT(ntc-compile-time CXX)
SET(CMAKE_BUILD_PARALLEL_LEVEL 8)

IF(NOT DEFINED CXX_STANDARD)
    SET(CMAKE_CXX_STANDARD 20)
ELSE()
    SET(CMAKE_CXX_STANDARD ${CXX_STANDARD})
ENDIF()
MESSAGE("Using C++ standard ${CMAKE_CXX_STANDARD}")

INCLUDE(cmake/StandardProjectSettings.cmake)
INCLUDE(cmake/PreventInSourceBuilds.cmake)

FIND_PACKAGE(Python COMPONENTS Interpreter REQUIRED)

# install PyPI Python package using pip
EXECUTE_PROCESS(COMMAND ${Python_EXECUTABLE} -m pip install --user --upgrade pip setuptools jinja2 conan)

# Link this 'library' to set the c++ standard / compile-time options requested
ADD_LIBRARY(project_options INTERFACE)
TARGET_COMPILE_FEATURES(project_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

IF(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    OPTION(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    IF(ENABLE_BUILD_WITH_TIME_TRACE)
        TARGET_COMPILE_OPTIONS(project_options INTERFACE -ftime-trace)
    ENDIF()
ENDIF()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
ADD_LIBRARY(project_warnings INTERFACE)

# enable cache system
INCLUDE(cmake/Cache.cmake)

# standard compiler warnings
INCLUDE(cmake/CompilerWarnings.cmake)
SET_PROJECT_WARNINGS(project_warnings)

# sanitizer options if supported by compiler
INCLUDE(cmake/Sanitizers.cmake)
ENABLE_SANITIZERS(project_options)

# enable doxygen
INCLUDE(cmake/Doxygen.cmake)
ENABLE_DOXYGEN()

# allow for static analysis options
INCLUDE(cmake/StaticAnalyzers.cmake)

OPTION(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
OPTION(ENABLE_TESTING "Enable Test Builds" ON)

# Very basic PCH example
OPTION(ENABLE_PCH "Enable Precompiled Headers" OFF)
IF(ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which is a good idea if any #define's change
    #
    # consider breaking this out per project as necessary
    TARGET_PRECOMPILE_HEADERS(
            project_options
            INTERFACE
            <vector>
            <utility>)
ENDIF()

FIND_PACKAGE(fmt REQUIRED)
FIND_PACKAGE(catch2 REQUIRED)

IF(ENABLE_TESTING)
    ENABLE_TESTING()
    MESSAGE("Building Tests.")
    ADD_SUBDIRECTORY(test)
ENDIF()

ADD_SUBDIRECTORY(source)

OPTION(ENABLE_UNITY "Enable Unity builds of projects" OFF)
IF(ENABLE_UNITY)
    # Add for any project you want to apply unity builds for
    SET_TARGET_PROPERTIES(TypeRegisterExample PROPERTIES UNITY_BUILD ON)
ENDIF()
