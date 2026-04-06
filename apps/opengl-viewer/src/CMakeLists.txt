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

set(OPENGL_VIEWER_SRC opengl-viewer.cpp)

add_executable(${TARGET} ${OPENGL_VIEWER_SRC})

target_compile_definitions(${TARGET} PRIVATE MGTT_OPENGL_VIEWER)

if(${BUILD_WEB})
    set(WEB_THIRD_PARTY_FOLDER ../../../assets/web/third-party)

    set(IMGUI_SRC
        ${WEB_THIRD_PARTY_FOLDER}/imgui/backends/imgui_impl_glfw.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/backends/imgui_impl_opengl3.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_widgets.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_tables.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_demo.cpp
        ${WEB_THIRD_PARTY_FOLDER}/imgui/imgui_draw.cpp
    )
    target_sources(${TARGET} PRIVATE ${IMGUI_SRC})

    # Emscripten -s flags must be passed as a single string, not as list items
    # in target_compile_options, otherwise clang treats each value as a filename
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -s WASM=1 -s USE_GLFW=3 \
        -s FULL_ES3=1 -fwasm-exceptions -s ASSERTIONS=2 -s SAFE_HEAP=1 \
        --preload-file ../../../../assets \
        --exclude-file ../../../../assets/web \
        --exclude-file ../../../../assets/scenes/glTF-Sample-Models \
        -s NO_EXIT_RUNTIME=1 -s TOTAL_MEMORY=1024MB -O3")

    target_include_directories(${TARGET} PRIVATE
        ${WEB_THIRD_PARTY_FOLDER}/glm
        ${WEB_THIRD_PARTY_FOLDER}/tinygltf
        ${WEB_THIRD_PARTY_FOLDER}/imgui
        ${WEB_THIRD_PARTY_FOLDER}/imgui/backends
    )

    target_link_libraries(${TARGET} PRIVATE window rendering)
else()
    find_package(IMGUI CONFIG REQUIRED)
    find_package(nfd   CONFIG REQUIRED)

    # Resolve nfd.h include path from the imported target's properties
    get_target_property(NFD_INCLUDE_DIRS nfd::nfd INTERFACE_INCLUDE_DIRECTORIES)

    target_include_directories(${TARGET} PRIVATE
        ${NFD_INCLUDE_DIRS}
    )

    target_link_libraries(${TARGET} PRIVATE
        window
        rendering
        imgui::imgui
        nfd::nfd
    )
    if(WIN32)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assets/
            PATTERN "glTF-Sample-Models" EXCLUDE
            PATTERN ".git" EXCLUDE)
    elseif(APPLE OR UNIX)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/
            PATTERN "glTF-Sample-Models" EXCLUDE
            PATTERN ".git" EXCLUDE)
    else()
        message(WARNING "Unsupported platform")
    endif()
endif()

target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/rendering/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/rendering/include/model
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/window/include
)

install(TARGETS ${TARGET})
