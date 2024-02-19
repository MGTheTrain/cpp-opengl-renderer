cmake_minimum_required(VERSION 3.10)
set(TARGET rendering)
project(${TARGET})

find_package(glm CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)

if(${GLM_FOUND} AND ${GLEW_FOUND})
    include_directories(../include) 
    set(RENDERING_SRC 
        opengl-shader.cpp
        gltf-scene-importer.cpp
        aabb.cpp
        material.cpp
        mesh-primitive.cpp
        mesh.cpp
        node.cpp
        scene.cpp
        texture.cpp)   
    add_library(${TARGET} STATIC ${RENDERING_SRC})
    target_link_libraries(${TARGET} PRIVATE GLEW::GLEW glm::glm-header-only)
endif()