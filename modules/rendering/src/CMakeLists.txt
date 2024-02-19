cmake_minimum_required(VERSION 3.10)

set(TARGET rendering)
project(${TARGET})

find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${OpenGL_FOUND} AND ${GLEW_FOUND} AND ${glm_FOUND} AND ${Stb_FOUND})
    include_directories(
        ${PROJECT_SOURCE_DIR}/../include
        ${PROJECT_SOURCE_DIR}/../include/model
        ${Stb_INCLUDE_DIR}
    )

    set(RENDERING_SRC 
        opengl-shader.cpp
        gltf-scene-importer.cpp
        model/aabb.cpp
        model/material.cpp
        model/mesh-primitive.cpp
        model/mesh.cpp
        model/node.cpp
        model/scene.cpp
        model/texture.cpp
    )   

    add_library(${TARGET} STATIC ${RENDERING_SRC})
    target_link_libraries(${TARGET} PRIVATE GLEW::GLEW glm::glm-header-only)
endif()