if(BUILD_LIB)
    add_subdirectory(src)
endif()

if(BUILD_TEST)
    add_subdirectory(test)
endif()