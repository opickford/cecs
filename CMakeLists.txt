cmake_minimum_required(VERSION 3.11)

project (cecs VERSION 0.1.2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CECS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CECS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

configure_file(
	${CECS_SOURCE_DIR}/cmake/version.h.in
	${CECS_BINARY_DIR}/include/cecs/version.h
)

add_subdirectory(src)

option(CECS_BUILD_EXAMPLES "Build examples" ON)
option(CECS_BUILD_TESTS "Build tests" ON)
option(CECS_INSTALL "Enable installation" ON)

if (CECS_BUILD_EXAMPLES)
    add_subdirectory(examples)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "example")
endif()

if (CECS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Optional install support
if(CECS_INSTALL)
    include(cmake/install.cmake)
endif()
