if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/alec-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

# Project is configured with no languages, so tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package alec)

install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT alec_Development
)

install(
    TARGETS alec_alec
    EXPORT alecTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Allow package maintainers to freely override the path for the configs
set(
    alec_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE alec_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(alec_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${alec_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT alec_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${alec_INSTALL_CMAKEDIR}"
    COMPONENT alec_Development
)

install(
    EXPORT alecTargets
    NAMESPACE alec::
    DESTINATION "${alec_INSTALL_CMAKEDIR}"
    COMPONENT alec_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
