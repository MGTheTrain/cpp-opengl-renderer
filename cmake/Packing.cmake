set(PROJECT_NAME ${PACKAGE_NAME} CACHE STRING "The project name")
set(CPACK_PACKAGE_NAME ${PACKAGE_NAME} CACHE STRING "The resulting package name")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple C++ apps and reusable static module libs"
    CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_VENDOR "Some company")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/packages")

set(CPACK_PACKAGING_INSTALL_PREFIX ${PACKAGING_INSTALL_PREFIX})

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_PACKAGE_CONTACT "marvin.gajek@outlook.com")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Install assets directory to /opt/mgtt/cpp-opengl-renderer/bin destination
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
install(DIRECTORY ${ASSETS_DIR} DESTINATION ${PACKAGING_INSTALL_PREFIX}/bin FILES_MATCHING PATTERN "*.*")

if(UNIX)
    # set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Marvin Gajek")
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
    set(CPACK_DEB_COMPONENT_INSTALL YES)
endif()

include(CPack)