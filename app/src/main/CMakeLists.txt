cmake_minimum_required(VERSION 3.22.1)
project("helloc" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#===================================================================
#  STEP 1: BUILD PROTOBUF FROM SOURCE
#===================================================================
include(FetchContent)

# Configure the Protobuf build for a lean, correct build on Android
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(gRPC_INSTALL OFF CACHE BOOL "" FORCE)

set(gRPC_ZLIB_PROVIDER "none" CACHE STRING "" FORCE)

# Declare the Protobuf dependency from GitHub using a stable version
FetchContent_Declare(
        grpc
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        GIT_TAG        v1.62.2 # A well-tested, stable version.
        GIT_SHALLOW    TRUE
)

FetchContent_GetProperties(grpc)
if(NOT grpc_POPULATED)
    FetchContent_Populate(grpc)
    add_subdirectory(${grpc_SOURCE_DIR} ${grpc_BINARY_DIR})
endif()

# ============================================================
# Proto + gRPC codegen for auth_service.proto
# ============================================================

# Host tools (not the Android cross-built protoc)
find_program(PROTOC_EXECUTABLE protoc)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

if(NOT PROTOC_EXECUTABLE)
    message(FATAL_ERROR "protoc not found on PATH. Install protoc for your host and try again.")
endif()

if(NOT GRPC_CPP_PLUGIN_EXECUTABLE)
    message(FATAL_ERROR "grpc_cpp_plugin not found on PATH. Install/build it for your host and try again.")
endif()

# Where your .proto lives (adjust if needed)
set(PROTO_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../MyGameServer/proto)
set(PROTO_FILE ${PROTO_DIR}/auth_service.proto)

get_filename_component(PROTO_FILENAME ${PROTO_FILE} NAME_WE)

set(PROTO_CPP_SRC "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_FILENAME}.pb.cc")
set(PROTO_CPP_HDR "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_FILENAME}.pb.h")
set(GRPC_CPP_SRC  "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_FILENAME}.grpc.pb.cc")
set(GRPC_CPP_HDR  "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_FILENAME}.grpc.pb.h")

add_custom_command(
        OUTPUT
        ${PROTO_CPP_SRC} ${PROTO_CPP_HDR}
        ${GRPC_CPP_SRC}  ${GRPC_CPP_HDR}
        COMMAND ${PROTOC_EXECUTABLE}
        --proto_path=${PROTO_DIR}
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
        ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating C++ and gRPC C++ sources from ${PROTO_FILE}"
        VERBATIM
)

add_library(auth_service_proto STATIC
        ${PROTO_CPP_SRC}
        ${PROTO_CPP_HDR}
        ${GRPC_CPP_SRC}
        ${GRPC_CPP_HDR}
)

# Include dir so you can #include "auth_service.pb.h" etc.
target_include_directories(auth_service_proto
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROTO_DIR}        # optional, if you want to #include the .proto for some reason
)

# Link against gRPC and Protobuf from the in-tree build
target_link_libraries(auth_service_proto
        PUBLIC
        grpc++              # NOT gRPC::grpc++
        protobuf::libprotobuf
)

find_package(oboe REQUIRED CONFIG)


add_definitions("-DGLM_FORCE_SIZE_T_LENGTH -DGLM_FORCE_RADIANS")

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${CMAKE_CURRENT_SOURCE_DIR}/data)

find_library(log-lib
        log)

find_library(z-lib z)


# Add glm as a library
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE cpp/glm)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -Wall -Werror")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(helloc
        SHARED
        cpp/helloc.cpp
        cpp/core/indexbuf.cpp
        cpp/core/input_util.cpp
        cpp/core/native_engine.cpp
        cpp/core/our_shader.cpp
        cpp/core/scene_manager.cpp
        cpp/core/sfxman.cpp
        cpp/core/sprite.cpp
        cpp/core/shader.cpp
        cpp/core/texture.cpp
        cpp/core/util.cpp
        cpp/core/vertexbuf.cpp
        cpp/scenes/splash_screen_scene.cpp
        cpp/scenes/title_screen_scene.cpp
        cpp/scenes/login_scene.cpp
        cpp/scenes/scene.cpp
        cpp/scenes/play_scene.cpp
        cpp/world/tileset.cpp
        cpp/world/tilemap.cpp
        cpp/core/asset_manager.cpp
        cpp/third_party/auth/auth_service.pb.cc
        cpp/third_party/auth/auth_service.grpc.pb.cc
)

include(CMakeFindDependencyMacro)
set(GLM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/glm)


target_include_directories(helloc PRIVATE cpp
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/third_party
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/third_party/auth
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/world
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
        ${z-lib}
        EGL
        GLESv2
        glm
        oboe::oboe
        grpc++           # C++ gRPC
        grpc             # core gRPC C library
        gpr              # gRPC support library
        protobuf::libprotobuf
        auth_service_proto

)




