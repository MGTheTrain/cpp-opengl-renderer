
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

if(NOT BUILD_WEB)
    set(TESTING_TARGET rendering_test)
    project(${TESTING_TARGET})

    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(tinyusdz CONFIG QUIET)

    set(RENDERING_TEST_SRC
        entrypoint.cpp
        opengl-shader-test.cpp
        gltf-scene-importer-test.cpp
        usd-scene-importer-test.cpp
        texture-manager-test.cpp
    )

    add_executable(${TESTING_TARGET} ${RENDERING_TEST_SRC})

    target_compile_definitions(${TESTING_TARGET} PRIVATE MGTT_RENDERING_TEST)

    target_include_directories(${TESTING_TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/model
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include
        ${Stb_INCLUDE_DIR}
    )

    if(tinyusdz_FOUND)
        target_include_directories(${TESTING_TARGET} PRIVATE ${TINYUSDZ_INCLUDE_DIR})
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${TESTING_TARGET} PRIVATE
            --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${TESTING_TARGET} PRIVATE --coverage)
    endif()

    target_link_libraries(${TESTING_TARGET} PRIVATE
        GTest::gtest
        GTest::gtest_main
        glfw
        rendering
    )

    add_test(NAME ${TESTING_TARGET} COMMAND $<TARGET_FILE:${TESTING_TARGET}>)

    if(WIN32)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/)
    elseif(APPLE OR UNIX)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/)
    else()
        message(WARNING "Unsupported platform")
    endif()

    install(TARGETS ${TESTING_TARGET})
endif()
