cmake_minimum_required(VERSION 3.10)
set(TARGET opengl-viewer)
project(${TARGET})   

option(BUILD_TEST "Compile source files for test executable" OFF)
option(BUILD_LIB "Compile module source files" OFF)
option(BUILD_MAIN "Compile source file of main executable and link compiled libraries" ON) 
option(BUILD_PACKAGE "Build the package" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(BUILD_TEST)
    include(CTestConfig)
endif()

# add_subdirectory(modules)
add_subdirectory(apps)

if(BUILD_PACKAGE)
    include(CustomFunctions)
    
    set(PROJECT_NAME "cpp-opengl-renderer")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}" CACHE STRING "The resulting package name")

    set(file_path "version") 
    file(READ "${file_path}" file_content)
    message(${file_content})
    split_version_string(${file_content})
    message("CMAKE_PROJECT_VERSION_MAJOR: ${CMAKE_PROJECT_VERSION_MAJOR}")
    message("CMAKE_PROJECT_VERSION_MINOR: ${CMAKE_PROJECT_VERSION_MINOR}")
    message("CMAKE_PROJECT_VERSION_PATCH: ${CMAKE_PROJECT_VERSION_PATCH}")

    if(REVISION_NUMBER)
        set(CMAKE_PROJECT_VERSION_TWEAK "-${REVISION_NUMBER}")
    endif()
    message("CMAKE_PROJECT_VERSION_TWEAK: ${CMAKE_PROJECT_VERSION_TWEAK}")

    set(PROJECT_VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}${CMAKE_PROJECT_VERSION_TWEAK} 
        CACHE STRING "The version of the debian package")

    include(Packing)
endif()
