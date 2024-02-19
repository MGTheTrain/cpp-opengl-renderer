cmake_minimum_required(VERSION 3.10)

set(TESTING_TARGET window_test)
project(${TESTING_TARGET})

add_definitions(-DMGTT_WINDOW_TEST)

enable_testing()
find_package(GTest CONFIG REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${GTest_FOUND})
    include_directories(${GTEST_INCLUDE_DIRS} ../include)
    set(WINDOW_TEST_SRC 
        entrypoint
        glfw-window-test.cpp)                        
    add_executable(${TESTING_TARGET} ${WINDOW_TEST_SRC})
    target_link_libraries(${TESTING_TARGET} PRIVATE GTest::gtest GTest::gtest_main window)
    add_test(NAME ${TESTING_TARGET} COMMAND $<TARGET_FILE:${TESTING_TARGET}>)
endif()