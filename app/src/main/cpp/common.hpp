//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_COMMON_HPP
#define HELLOC_COMMON_HPP
extern "C" {
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <errno.h>
#include <jni.h>
#include <stdlib.h>
#include <unistd.h>
}
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define DEBUG_TAG "EndlessTunnel:Native"
#define LOGD(...) \
  ((void)__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, __VA_ARGS__))
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, DEBUG_TAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, DEBUG_TAG, __VA_ARGS__))
#define ABORT_GAME              \
  {                             \
    LOGE("*** GAME ABORTING."); \
    *((volatile char*)0) = 'a'; \
  }
#define DEBUG_BLIP LOGD("[ BLIP ]: %s:%d", __FILE__, __LINE__)

#define MY_ASSERT(cond)                    \
  {                                        \
    if (!(cond)) {                         \
      LOGE("ASSERTION FAILED: %s", #cond); \
      ABORT_GAME;                          \
    }                                      \
  }

#include "our_key_codes.hpp"
#endif //HELLOC_COMMON_HPP
