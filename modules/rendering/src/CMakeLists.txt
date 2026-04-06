# The MIT License
#
# Copyright (c) 2026 MGTheTrain
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

set(RENDERING_SRC
    external-tinygltf-impl.cpp
    opengl-shader.cpp
    gltf-scene-importer.cpp
    usd-scene-importer.cpp
    scene-uploader.cpp
    texture-manager.cpp
    model/aabb.cpp
    model/material.cpp
    model/mesh-primitive.cpp
    model/mesh.cpp
    model/node.cpp
    model/scene.cpp
    model/texture.cpp
)

find_package(tinyusdz CONFIG QUIET)

add_library(${TARGET} STATIC ${RENDERING_SRC})

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/../include/model
    ${PROJECT_SOURCE_DIR}/../../common/include
)

if(BUILD_WEB)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -s WASM=1 -s FULL_ES3=1 \
        -fwasm-exceptions -s ASSERTIONS=2 -s SAFE_HEAP=1 \
        -s NO_EXIT_RUNTIME=1 -s TOTAL_MEMORY=1024MB -O3")

    set(WEB_THIRD_PARTY_FOLDER ../../../assets/web/third-party)
    target_include_directories(${TARGET} PRIVATE
        ${WEB_THIRD_PARTY_FOLDER}/glm
        ${WEB_THIRD_PARTY_FOLDER}/tinygltf
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW   CONFIG REQUIRED)
    find_package(glm    CONFIG REQUIRED)
    find_package(Stb    REQUIRED)
    find_path(TINYGLTF_INCLUDE_DIR "tiny_gltf.h" REQUIRED)

    target_include_directories(${TARGET} PRIVATE
        ${Stb_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${TINYGLTF_INCLUDE_DIR}
    )

    target_link_libraries(${TARGET} PRIVATE
        OpenGL::GL
        GLEW::GLEW
        glm::glm-header-only
    )
endif()

if(tinyusdz_FOUND AND NOT BUILD_WEB)
message(STATUS "Using tinyusdz via vcpkg")
target_include_directories(${TARGET} PRIVATE ${TINYUSDZ_INCLUDE_DIR})
target_link_libraries(${TARGET} PRIVATE tinyusdz::tinyusdz_static)
else()
message(STATUS "tinyusdz not found — USD support disabled")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${TARGET} PRIVATE
        --coverage -fprofile-arcs -ftest-coverage)
    # No target_link_options here
endif()

install(TARGETS ${TARGET})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../include/
    DESTINATION include/rendering
    FILES_MATCHING PATTERN "*.h"
)
