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

set(TARGET window)
project(${TARGET})

set(WINDOW_SRC glfw-context.cpp glfw-window.cpp)

add_library(${TARGET} STATIC ${WINDOW_SRC})

if(${BUILD_WEB})
    # Emscripten -s flags must be passed as a single string, not as list items
    # in target_compile_options, otherwise clang treats each value as a filename
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -s WASM=1 -s USE_GLFW=3 \
        -fwasm-exceptions -s ASSERTIONS=2 -s SAFE_HEAP=1 \
        -s NO_EXIT_RUNTIME=1 -s TOTAL_MEMORY=1024MB -O3")
else()
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(${TARGET} PUBLIC glfw)
endif()

target_include_directories(${TARGET} PUBLIC
    ${PROJECT_SOURCE_DIR}/../include
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${TARGET} PRIVATE
        --coverage -fprofile-arcs -ftest-coverage)
endif()

install(TARGETS ${TARGET})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../include/
    DESTINATION include/window
    FILES_MATCHING PATTERN "*.h"
)
