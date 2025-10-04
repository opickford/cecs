cmake_minimum_required(VERSION 3.8)

project (cecs VERSION 1.0.0 LANGUAGES C)

option(CECS_BUILD_EXAMPLES "Build examples" ON)
#option(CECS_BUILD_TESTS "Build tests" OFF) # TODO: Tests.
option(CECS_INSTALL "Enable installation" ON)

# Set output directories for all targets, makes it a bit neater.
# TODO: DO we need these?
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if (CECS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# TODO:
#if (CECS_BUILD_TEST)
    #add_subdirectory(tests)
#endif()

# Optional install support
if(CECS_INSTALL)
    include(cmake/install.cmake)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "example")
