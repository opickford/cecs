# Export targets
install(TARGETS cecs EXPORT cecsTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)

install(EXPORT cecsTargets
    FILE cecsTargets.cmake
    NAMESPACE cecs::
    DESTINATION lib/cmake/cecs
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/cecsConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/cecsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/cecs
)

install(FILES
    "${CMAKE_BINARY_DIR}/cecsConfig.cmake"
    DESTINATION lib/cmake/cecs
)