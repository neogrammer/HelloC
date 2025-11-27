//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_JNI_UTIL_HPP
#define HELLOC_JNI_UTIL_HPP


#include <jni.h>

struct JniSetup {
    jclass clazz;  // activity class
    jobject thiz;  // activity object
    JNIEnv* env;   // jni env
};

// Does JNI setup (if needed) and returns a struct with convenience objects.
struct JniSetup* GetJNISetup();

#endif //HELLOC_JNI_UTIL_HPP
