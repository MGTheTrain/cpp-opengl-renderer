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

if(${BUILD_WEB})
    set(WEB_THIRD_PARTY_FOLDER ../../../assets/web/third-party)
    set(CMAKE_CXX_FLAGS "-s WASM=1 -std=c++17 -s FULL_ES3=1 -fwasm-exceptions -s ASSERTIONS=2 \
    -s SAFE_HEAP=1 -s NO_EXIT_RUNTIME=1 -s TOTAL_MEMORY=1024MB -O3")
    include_directories(
        ${WEB_THIRD_PARTY_FOLDER}/glm
        ${WEB_THIRD_PARTY_FOLDER}/tinygltf # has stb_image.h
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(Stb REQUIRED)
    find_path(tinygltf "tiny_gltf.h")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories(
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../include/model
    ${Stb_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
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
if(${BUILD_WEB})
    # /
else()
    target_link_libraries(${TARGET} PRIVATE GLEW::GLEW glm::glm-header-only)
endif()
install(TARGETS ${TARGET})