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
set(TARGET opengl_viewer)
project(${TARGET})

add_definitions(-DMGTT_OPENGL_VIEWER)

if(${BUILD_WEB})
    set(PROJECT_ROOT ../../..)
    set(WEB_THIRD_PARTY_FOLDER ${PROJECT_ROOT}/assets/web/third-party)
    set(CMAKE_CXX_FLAGS "-s WASM=1 -std=c++17 -s FULL_ES3=1 -s USE_GLFW=3 --preload-file ../../../../assets \
    --exclude-file ../../../../assets/web --exclude-file ../../../../assets/scenes/glTF-Sample-Models \
    -fwasm-exceptions -s ASSERTIONS=2 -s SAFE_HEAP=1 -s NO_EXIT_RUNTIME=1 -s TOTAL_MEMORY=1024MB -O3")
    include_directories(
        ${WEB_THIRD_PARTY_FOLDER}/glm
        ${WEB_THIRD_PARTY_FOLDER}/tinygltf # has stb_image.h
        ${WEB_THIRD_PARTY_FOLDER}/imgui
        ${WEB_THIRD_PARTY_FOLDER}/imgui/backends
    )
    set(IMGUI_SRC
        ${WEB_THIRD_PARTY_FOLDER}/imgui/backends/imgui_impl_glfw.cpp     
        ${WEB_THIRD_PARTY_FOLDER}/imgui/backends/imgui_impl_opengl3.cpp          
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_widgets.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui.cpp                       
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_tables.cpp                       
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_demo.cpp              
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_draw.cpp      
    )
else()
    find_package(IMGUI CONFIG REQUIRED)
    find_package(unofficial-nativefiledialog CONFIG REQUIRED)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories(../include
                    ../../../modules/rendering/include
                    ../../../modules/rendering/include/model
                    ../../../modules/window/include
                    ${IMGUI_INCLUDE_DIRS}
)
set(OPENGL_VIEWER_SRC opengl-viewer.cpp)                        
if(${BUILD_WEB})
    add_executable(${TARGET} ${OPENGL_VIEWER_SRC} ${IMGUI_SRC})
    target_link_libraries(${TARGET} PRIVATE window rendering)
else()
    add_executable(${TARGET} ${OPENGL_VIEWER_SRC})
    if(WIN32)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/shader/core/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/shader/core/)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/texture/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/texture/)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/scenes/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/scenes/)
    elseif(APPLE OR UNIX)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/shader/core/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/shader/core/)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/texture/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/texture/)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/scenes/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/scenes/)
    else()
        message(WARNING "Unsupported platform")
    endif()
    target_link_libraries(${TARGET} PRIVATE window rendering imgui::imgui unofficial::nativefiledialog::nfd)
endif()

install(TARGETS ${TARGET})