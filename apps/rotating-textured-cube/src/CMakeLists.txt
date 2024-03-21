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
set(TARGET rotating_textured_cube)
project(${TARGET})

add_definitions(-DMGTT_ROTATING_TEXTURED_CUBE)

set(ROTATING_TEXTURED_CUBE_SRC rotating-textured-cube.cpp)      

set(PROJECT_ROOT ../../..)
set(WEB_THIRD_PARTY_FOLDER ${PROJECT_ROOT}/assets/web/third-party)
if(ANDROID)
    include_directories(
        ../include
        ${PROJECT_ROOT}/modules/rendering/include
        ${PROJECT_ROOT}/modules/rendering/include/model
        ${WEB_THIRD_PARTY_FOLDER}/glm
        ${WEB_THIRD_PARTY_FOLDER}/tinygltf # has stb_image.h
    )
    set(RENDERING_SRC 
        ${PROJECT_ROOT}/modules/rendering/src/opengl-shader.cpp
        ${PROJECT_ROOT}/modules/rendering/src/gltf-scene-importer.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/aabb.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/material.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/mesh-primitive.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/mesh.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/node.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/scene.cpp
        ${PROJECT_ROOT}/modules/rendering/src/model/texture.cpp
    )   
    add_library(${TARGET} SHARED ${ROTATING_TEXTURED_CUBE_SRC} ${RENDERING_SRC})
    target_link_libraries(${TARGET} android EGL GLESv3 log)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ROOT}/assets/shader/es/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/shader/es/)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ROOT}/assets/texture/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/texture/)
    install(TARGETS ${TARGET} LIBRARY DESTINATION lib)
else()
    if(${BUILD_WEB})
        set(CMAKE_CXX_FLAGS "-s WASM=1 -std=c++17 -s FULL_ES3=1 -s USE_GLFW=3 --preload-file ../../../../assets \
        --exclude-file ../../../../assets/web --exclude-file ../../../../assets/scenes/glTF-Sample-Models -fwasm-exceptions \
        -s ASSERTIONS=2 -s SAFE_HEAP=1 -s NO_EXIT_RUNTIME=1 -s TOTAL_MEMORY=1024MB -O3")
        include_directories(
            ../include
            ${PROJECT_ROOT}/modules/rendering/include
            ${PROJECT_ROOT}/modules/rendering/include/model
            ${PROJECT_ROOT}/modules/window/include
            ${WEB_THIRD_PARTY_FOLDER}/glm
            ${WEB_THIRD_PARTY_FOLDER}/tinygltf # has stb_image.h
        )
        add_executable(${TARGET} ${ROTATING_TEXTURED_CUBE_SRC})
        target_link_libraries(${TARGET} PRIVATE window rendering)
    else()
        # WIN32, APPLE, UNIX
        find_package(GLEW CONFIG REQUIRED)
        include_directories(
            ../include
            ${PROJECT_ROOT}/modules/rendering/include
            ${PROJECT_ROOT}/modules/rendering/include/model
            ${PROJECT_ROOT}/modules/window/include
            ${GLEW_INCLUDE_DIRS}
        )
        add_executable(${TARGET} ${ROTATING_TEXTURED_CUBE_SRC})
        target_link_libraries(${TARGET} PRIVATE window rendering)

        if(WIN32)
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ROOT}/assets/shader/core/
                DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/shader/core/)
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ROOT}/assets/texture/
                DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/texture/)
            install(TARGETS ${TARGET})
        elseif(APPLE OR UNIX)
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ROOT}/assets/shader/core/
                DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/shader/core/)
            file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_ROOT}/assets/texture/
                DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/texture/)
            install(TARGETS ${TARGET})
        else()
            message(WARNING "Unsupported platform")
        endif()
    endif()
endif()