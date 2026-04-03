set(CPACK_PACKAGE_NAME ${PACKAGE_NAME} CACHE STRING "The resulting package name")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform C++ OpenGL renderer with PBR pipeline and glTF support"
    CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_VENDOR "MGTheTrain")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/packages")

set(CPACK_PACKAGING_INSTALL_PREFIX ${PACKAGING_INSTALL_PREFIX})

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_PACKAGE_CONTACT "marvin.gajek@outlook.com")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(BUILD_WEB)
    install(FILES
        ${CMAKE_BINARY_DIR}/apps/opengl-viewer/src/opengl_viewer.js
        ${CMAKE_BINARY_DIR}/apps/opengl-viewer/src/opengl_viewer.wasm
        ${CMAKE_BINARY_DIR}/apps/opengl-viewer/src/opengl_viewer.data
        ${CMAKE_BINARY_DIR}/apps/rotating-textured-cube/src/rotating_textured_cube.js
        ${CMAKE_BINARY_DIR}/apps/rotating-textured-cube/src/rotating_textured_cube.wasm
        ${CMAKE_BINARY_DIR}/apps/rotating-textured-cube/src/rotating_textured_cube.data
        DESTINATION web
    )
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/web/html-js-wasm
        DESTINATION web
        FILES_MATCHING PATTERN "*.*"
    )
    set(CPACK_SYSTEM_NAME "web")
else()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/scenes/water-bottle
        DESTINATION bin/assets/scenes
        FILES_MATCHING PATTERN "*.*"
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/shader
        DESTINATION bin/assets
        FILES_MATCHING PATTERN "*.*"
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/texture
        DESTINATION bin/assets
        FILES_MATCHING PATTERN "*.*"
    )

    if(UNIX)
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Marvin Gajek")
        set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
        set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
        set(CPACK_DEB_COMPONENT_INSTALL YES)
    endif()
endif()

include(CPack)
