# [Company Name] Software License
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
set(TARGET cpp-opengl-renderer)
project(${TARGET})   
set(CMAKE_CXX_STANDARD 17)

option(BUILD_LIB "Compile module source files" ON)
option(BUILD_TEST "Compile source files for test executable" ON)
option(BUILD_APP "Compile source file of main executable and link compiled libraries" OFF) 
option(BUILD_PACKAGE "Build the package" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AzureVcpkg)
az_vcpkg_integrate()

if(BUILD_TEST)
    include(CTestConfig)
endif()

add_subdirectory(modules)
add_subdirectory(apps)

if(BUILD_PACKAGE)
    include(CustomFunctions)
    
    set(PROJECT_NAME "cpp-opengl-renderer")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}" CACHE STRING "The resulting package name")

    set(file_path "version") 
    file(READ "${file_path}" file_content)
    message(${file_content})
    split_version_string(${file_content})
    message("CMAKE_PROJECT_VERSION_MAJOR: ${CMAKE_PROJECT_VERSION_MAJOR}")
    message("CMAKE_PROJECT_VERSION_MINOR: ${CMAKE_PROJECT_VERSION_MINOR}")
    message("CMAKE_PROJECT_VERSION_PATCH: ${CMAKE_PROJECT_VERSION_PATCH}")

    if(REVISION_NUMBER)
        set(CMAKE_PROJECT_VERSION_TWEAK "-${REVISION_NUMBER}")
    endif()
    message("CMAKE_PROJECT_VERSION_TWEAK: ${CMAKE_PROJECT_VERSION_TWEAK}")

    set(PROJECT_VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}${CMAKE_PROJECT_VERSION_TWEAK} 
        CACHE STRING "The version of the debian package")

    include(Packing)
endif()
