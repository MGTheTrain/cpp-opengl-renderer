cmake_minimum_required(VERSION 3.10)
set(TESTING_TARGET entrypoint)
project(${TESTING_TARGET})

find_package(GTest CONFIG REQUIRED)

if(${GTest_FOUND})
    include_directories(${GTEST_INCLUDE_DIRS} ../include)
    set(RENDERING_TEST_SRC 
        opengl-shader-test.cpp
        gltf-scene-importer-test.cpp)                        
    add_executable(${TESTING_TARGET} ${TESTING_TARGET}.cpp ${RENDERING_TEST_SRC})
    target_link_libraries(${TESTING_TARGET} GTest::gtest -pthread rendering)
    add_test(NAME ${TESTING_TARGET} COMMAND $<TARGET_FILE:${TESTING_TARGET}>)
endif()