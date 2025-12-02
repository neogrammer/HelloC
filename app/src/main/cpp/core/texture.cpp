#include <android/asset_manager.h>
#include <android/bitmap.h>
#include "texture.hpp"
#include "native_engine.hpp"

Texture::~Texture() {
    if (mTextureH) {
        glDeleteTextures(1, &mTextureH);
        mTextureH = 0;
    }
}

void Texture::InitFromAsset(const char* assetName) {

    std::string path = "textures/";
    path.append(std::string(assetName));
    path.append(".png");


    JNIEnv* env = NativeEngine::GetInstance()->GetJniEnv();
    jobject activity = NativeEngine::GetInstance()->GetActivity();
    jclass activityClass = env->GetObjectClass(activity);

    jmethodID getAssetsMethod = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManager = env->CallObjectMethod(activity, getAssetsMethod);

//    jstring assetNameJString = env->NewStringUTF(assetName);
//    jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");
//    jmethodID decodeStreamMethod = env->GetStaticMethodID(bitmapFactoryClass, "decodeStream", "(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");

    jclass assetManagerClass = env->GetObjectClass(assetManager);
    jmethodID openMethod = env->GetMethodID(assetManagerClass, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
    jstring assetNameJString = env->NewStringUTF(path.c_str());
    jobject inputStream = env->CallObjectMethod(assetManager, openMethod, assetNameJString);
    // --- Force ARGB_8888 decoding to preserve transparency ---
    // 1. Create BitmapFactory.Options object
    jclass optionsClass = env->FindClass("android/graphics/BitmapFactory$Options");
    jobject options = env->NewObject(optionsClass, env->GetMethodID(optionsClass, "<init>", "()V"));
    // 2. Get Bitmap.Config.ARGB_8888 enum value
    jclass configClass = env->FindClass("android/graphics/Bitmap$Config");
    jfieldID argb8888Field = env->GetStaticFieldID(configClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jobject argb8888 = env->GetStaticObjectField(configClass, argb8888Field);
    // 3. Set inPreferredConfig field on the options object
    jfieldID inPreferredConfigField = env->GetFieldID(optionsClass, "inPreferredConfig", "Landroid/graphics/Bitmap$Config;");
    env->SetObjectField(options, inPreferredConfigField, argb8888);
    // 4. Get the correct decodeStream method
    jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");
    jmethodID decodeStreamMethod = env->GetStaticMethodID(bitmapFactoryClass, "decodeStream", "(Ljava/io/InputStream;Landroid/graphics/Rect;Landroid/graphics/BitmapFactory$Options;)Landroid/graphics/Bitmap;");
    // 5. Decode the bitmap with the specified options
    jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass, decodeStreamMethod, inputStream, NULL, options);
    // ---------------------------------------------------------
    //jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass, decodeStreamMethod, inputStream);
    if (inputStream == nullptr) {
        LOGE("Failed to open asset: %s", path.c_str());
        return;
    }
    if (bitmap == nullptr) {
        LOGE("Failed to decode asset: %s", path.c_str());
        return;
    }

    AndroidBitmapInfo info;
    void* pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

//    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 && info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
//        LOGE("Bitmap format is not RGBA_8888 or RGB_565 !");
//        return;
//    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888, transparency may be lost.");
        // We can still try to render it, but it might not be what's expected.
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    InitFromRawRGB(info.width, info.height, info.format == ANDROID_BITMAP_FORMAT_RGBA_8888, (unsigned char*)pixels);

    AndroidBitmap_unlockPixels(env, bitmap);

    env->DeleteLocalRef(bitmap);
    env->DeleteLocalRef(inputStream);
    env->DeleteLocalRef(assetManager);
    env->DeleteLocalRef(assetNameJString);
    env->DeleteLocalRef(options);
    env->DeleteLocalRef(argb8888);
}

void Texture::InitFromRawRGB(int width, int height, bool hasAlpha,
                             const unsigned char* data) {
    mWidth = width;
    mHeight = height;
    GLenum format = hasAlpha ? GL_RGBA : GL_RGB;

    glGenTextures(1, &mTextureH);
    glBindTexture(GL_TEXTURE_2D, mTextureH);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(int unit) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, mTextureH);
}
//
//bool Texture::InitFromBuffer(const unsigned char* buffer, int bufferSize)
//{
//    if (mTextureH != 0) {
//        glDeleteTextures(1, &mTextureH);
//        mTextureH = 0;
//    }
//    JNIEnv* env = NativeEngine::GetInstance()->GetJniEnv();
//
//    // Convert the C++ buffer into a Java byte array
//    jbyteArray javaBuffer = env->NewByteArray(bufferSize);
//    if (javaBuffer == nullptr) {
//    LOGE("Failed to create new jbyteArray.");
//    return false;
//    }
//    env->SetByteArrayRegion(javaBuffer, 0, bufferSize, reinterpret_cast<const jbyte*>(buffer));
//
//    // Get the BitmapFactory class
//    jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");
//    if (bitmapFactoryClass == nullptr) {
//    LOGE("Failed to find class 'android/graphics/BitmapFactory'");
//    env->DeleteLocalRef(javaBuffer);
//    return false;
//    }
//
//    // Get the decodeByteArray method
//    jmethodID decodeMethod = env->GetStaticMethodID(bitmapFactoryClass, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
//    if (decodeMethod == nullptr) {
//    LOGE("Failed to find method 'decodeByteArray'");
//    env->DeleteLocalRef(javaBuffer);
//    return false;
//    }
//
//    // Call the decode method
//    jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass, decodeMethod, javaBuffer, 0, bufferSize);
//
//    // Clean up the java byte array
//    env->DeleteLocalRef(javaBuffer);
//
//    if (bitmap == nullptr) {
//    LOGE("Failed to decode bitmap from buffer.");
//    return false;
//    }
//
//    // The rest of this is the same as your existing InitFromAsset
//    AndroidBitmapInfo info;
//    void* pixels;
//    int ret;
//
//    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
//    LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
//    env->DeleteLocalRef(bitmap);
//    return false;
//    }
//
//    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 && info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
//    LOGE("Bitmap format is not RGBA_8888 or RGB_565 !");
//    env->DeleteLocalRef(bitmap);
//    return false;
//    }
//
//    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
//    LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
//    env->DeleteLocalRef(bitmap);
//    return false;
//    }
//
//    InitFromRawRGB(info.width, info.height, info.format == ANDROID_BITMAP_FORMAT_RGBA_8888, (unsigned char*)pixels);
//
//    AndroidBitmap_unlockPixels(env, bitmap);
//
//    // Clean up the bitmap object
//    env->DeleteLocalRef(bitmap);
//
//    return true;
//}

void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
