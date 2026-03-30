include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Export targets
install(TARGETS cecs 
    EXPORT cecsTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Copy contents of include dir.
install(DIRECTORY include/ 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# version.h lives in the build dir, so must be installed separately to the other headers.
# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/chds/version.h
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/chds
# )

install(EXPORT cecsTargets
    FILE cecsTargets.cmake
    NAMESPACE cecs::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cecs
)

# Generate cecsConfig for find_package.
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cecsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cecsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cecs
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cecsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cecsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cecsConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cecs
)