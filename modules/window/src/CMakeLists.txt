cmake_minimum_required(VERSION 3.10)
set(TARGET window)
project(${TARGET})

find_package(glfw3 CONFIG REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${glfw3_FOUND})
    include_directories(../include) 

    set(WINDOW_SRC 
        glfw-window.cpp
    ) 

    add_library(${TARGET} STATIC ${WINDOW_SRC})
    target_link_libraries(${TARGET} PRIVATE glfw)
endif()