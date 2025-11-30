#ifndef HELLOC_SPRITE_HPP
#define HELLOC_SPRITE_HPP

#include <string>
#include "int_rect.hpp"

// Forward declarations
class OurShader;
class SimpleGeom;
class Texture;

class Sprite {
public:
    Sprite(const std::string& textureName, const IntRect& textureRect);
    ~Sprite();

    void StartGraphics();
    void KillGraphics();

    void Draw(OurShader* shader, float x, float y);

private:
    std::string mTextureName;
    IntRect mTextureRect;
    Texture* mTexture;
    SimpleGeom* mGeom;
};

#endif //HELLOC_SPRITE_HPP
