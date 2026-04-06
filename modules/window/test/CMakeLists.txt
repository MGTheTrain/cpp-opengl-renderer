cmake_minimum_required(VERSION 3.10)

if(NOT ${BUILD_WEB})
    set(TESTING_TARGET window_test)
    project(${TESTING_TARGET})

    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    set(WINDOW_TEST_SRC
        entrypoint.cpp
        glfw-window-test.cpp)

    add_executable(${TESTING_TARGET} ${WINDOW_TEST_SRC})

    target_compile_definitions(${TESTING_TARGET} PRIVATE MGTT_WINDOW_TEST)

    target_include_directories(${TESTING_TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${TESTING_TARGET} PRIVATE
            --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${TESTING_TARGET} PRIVATE --coverage)
    endif()

    target_link_libraries(${TESTING_TARGET} PRIVATE
        GTest::gtest
        GTest::gtest_main
        window
    )

    add_test(NAME ${TESTING_TARGET} COMMAND $<TARGET_FILE:${TESTING_TARGET}>)

    install(TARGETS ${TESTING_TARGET})
endif()
