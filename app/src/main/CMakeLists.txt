cmake_minimum_required(VERSION 3.22.1)
project("helloc" LANGUAGES C CXX)

find_package(oboe REQUIRED CONFIG)


add_definitions("-DGLM_FORCE_SIZE_T_LENGTH -DGLM_FORCE_RADIANS")

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${CMAKE_CURRENT_SOURCE_DIR}/data)

find_library(log-lib
        log)


# Add glm as a library
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE cpp/glm)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -Wall -Werror")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(helloc
        SHARED
        cpp/helloc.cpp
        cpp/core/anim.cpp
        cpp/core/ascii_to_geom.cpp
        cpp/core/indexbuf.cpp
        cpp/core/input_util.cpp
        cpp/core/jni_util.cpp
        cpp/core/native_engine.cpp
        cpp/core/our_shader.cpp
        cpp/core/scene_manager.cpp
        cpp/core/sfxman.cpp
        cpp/core/sprite.cpp
        cpp/core/welcome_scene.cpp
        cpp/core/shader.cpp
        cpp/core/shape_renderer.cpp
        cpp/core/text_renderer.cpp
        cpp/core/texture.cpp
        cpp/scenes/ui_scene.cpp
        cpp/core/util.cpp
        cpp/core/vertexbuf.cpp
        cpp/scenes/splash_screen_scene.cpp
        cpp/scenes/title_screen_scene.cpp
        cpp/scenes/scene.cpp
        cpp/scenes/play_scene.cpp
        cpp/scenes/dialog_scene.cpp
)

include(CMakeFindDependencyMacro)
set(GLM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/glm)


target_include_directories(helloc PRIVATE cpp
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/third_party
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/core
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/data
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/scenes
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/glm
        ${GLM_INCLUDE_DIRS}
)
target_link_options(helloc PRIVATE "-Wl,-z,max-page-size=16384")
target_link_libraries(helloc
        PRIVATE
        android
        jnigraphics
        native_app_glue
        atomic
        ${log-lib}
        EGL
        GLESv2
        glm
        oboe::oboe
        OpenSLES
)




