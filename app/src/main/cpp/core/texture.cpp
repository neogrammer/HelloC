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
    JNIEnv* env = NativeEngine::GetInstance()->GetJniEnv();
    jobject activity = NativeEngine::GetInstance()->GetActivity();
    jclass activityClass = env->GetObjectClass(activity);

    jmethodID getAssetsMethod = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManager = env->CallObjectMethod(activity, getAssetsMethod);

    jstring assetNameJString = env->NewStringUTF(assetName);
    jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");
    jmethodID decodeStreamMethod = env->GetStaticMethodID(bitmapFactoryClass, "decodeStream", "(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");

    jclass assetManagerClass = env->GetObjectClass(assetManager);
    jmethodID openMethod = env->GetMethodID(assetManagerClass, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
    jobject inputStream = env->CallObjectMethod(assetManager, openMethod, assetNameJString);

    jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass, decodeStreamMethod, inputStream);

    if (bitmap == nullptr) {
        LOGE("Failed to decode asset: %s", assetName);
        return;
    }

    AndroidBitmapInfo info;
    void* pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 && info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGBA_8888 or RGB_565 !");
        return;
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
}

void Texture::InitFromRawRGB(int width, int height, bool hasAlpha,
                             const unsigned char* data) {
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

void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
