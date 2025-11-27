cmake_minimum_required(VERSION 3.22.1)
project(helloc)

add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue)

find_library(log-lib
    log)


# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -Wall -Werror")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(helloc
        SHARED
        src/main/cpp/helloc.cpp
        src/main/cpp/core/Renderer.cpp)

target_link_libraries(helloc
    android
        native_app_glue
        ${log-lib}
        EGL
        GLESv3
)