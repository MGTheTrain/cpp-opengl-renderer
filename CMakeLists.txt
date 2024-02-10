cmake_minimum_required(VERSION 3.10)
set(TARGET cpp-opengl-renderer)
project(${TARGET})   

option(BUILD_TEST "Compile source files for test executable" OFF)
option(BUILD_LIB "Compile module source files" OFF)
option(BUILD_APP "Compile source file of main executable and link compiled libraries" ON) 
option(BUILD_PACKAGE "Build the package" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AzureVcpkg)
az_vcpkg_integrate()

# find_package(GLEW REQUIRED)
# target_link_libraries(main PRIVATE GLEW::GLEW)

# # this is heuristically generated, and may not be correct
# find_package(glfw3 CONFIG REQUIRED)
# target_link_libraries(main PRIVATE glfw)

# find_package(glm CONFIG REQUIRED)
# target_link_libraries(main PRIVATE glm::glm-header-only)

# # this is heuristically generated, and may not be correct
# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(main PRIVATE imgui::imgui)

# find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
# target_include_directories(main PRIVATE ${TINYGLTF_INCLUDE_DIRS})

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
