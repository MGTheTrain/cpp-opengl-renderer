cmake_minimum_required(VERSION 3.10)
set(TESTING_TARGET rendering_test)
project(${TESTING_TARGET})

add_definitions(-DMGTT_RENDERING_TEST)

enable_testing()
find_package(GTest CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${GTest_FOUND} AND ${glfw3_FOUND})
    include_directories(${GTEST_INCLUDE_DIRS} ../include)
    include_directories(${GTEST_INCLUDE_DIRS} ../include/model)
    set(RENDERING_TEST_SRC 
        entrypoint.cpp
        opengl-shader-test.cpp
        gltf-scene-importer-test.cpp)                        
    add_executable(${TESTING_TARGET} ${RENDERING_TEST_SRC})
    target_link_libraries(${TESTING_TARGET} PRIVATE GTest::gtest GTest::gtest_main glfw rendering)
    add_test(NAME ${TESTING_TARGET} COMMAND $<TARGET_FILE:${TESTING_TARGET}>)

    # @note On Windows OS destination path had to be `${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/`
    # We might need to fix this in that tests work for all targeted systems
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../../assets/ 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets/)
endif()
