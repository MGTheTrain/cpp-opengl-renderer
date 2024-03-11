# The MIT License
#
# Copyright (c) 2024 MGTheTrain
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.10)

set(TARGET rendering)
project(${TARGET})

find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${OpenGL_FOUND} AND ${GLEW_FOUND} AND ${glm_FOUND} AND ${Stb_FOUND})
    include_directories(
        ${PROJECT_SOURCE_DIR}/../include
        ${PROJECT_SOURCE_DIR}/../include/model
        ${Stb_INCLUDE_DIR}
        ${TINYGLTF_INCLUDE_DIRS}
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
    install(TARGETS ${TARGET})
endif()