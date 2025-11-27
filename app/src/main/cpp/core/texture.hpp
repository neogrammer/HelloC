//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_TEXTURE_HPP
#define HELLOC_TEXTURE_HPP



#include "common.hpp"

/* Represents an OpenGL texture */
class Texture {
private:
    GLuint mTextureH;

public:
    inline Texture() { mTextureH = 0; }
    ~Texture();

    // Initialize from raw RGB data. If hasAlpha is true, then it's 4 bytes per
    // pixel (RGBA), otherwise it's interpreted as 3 bytes per pixel (RGB).
    void InitFromRawRGB(int width, int height, bool hasAlpha,
                        const unsigned char *data);

    // Initialize from an asset.
    void InitFromAsset(const char* assetName);

    void Bind(int unit);
    void Unbind();
};


#endif //HELLOC_TEXTURE_HPP
