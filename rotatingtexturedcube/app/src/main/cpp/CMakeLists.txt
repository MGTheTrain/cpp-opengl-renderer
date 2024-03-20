# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html
cmake_minimum_required(VERSION 3.10.2)

project("rtc")

add_library(
        rtc
        SHARED
        native-lib.cpp)
find_library(
        log-lib
        log)

target_link_libraries(
        rtc
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})