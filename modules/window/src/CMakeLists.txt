cmake_minimum_required(VERSION 3.10)
set(TARGET window)
project(${TARGET})

find_package(glfw3 CONFIG REQUIRED)

if(${GLFW_FOUND})
    include_directories(../include) 
    add_library(${TARGET} STATIC glfw-window.cpp)
    target_link_libraries(${TARGET} PRIVATE glfw)
endif()