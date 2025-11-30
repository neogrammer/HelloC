#ifndef HELLOC_SPRITE_HPP
#define HELLOC_SPRITE_HPP

#include <string>
#include "int_rect.hpp"
#include "glm/glm.hpp"

// Forward declarations
class OurShader;
class SimpleGeom;
class Texture;

class Sprite {
public:
    // Constructor for a sprite that uses the entire texture
    Sprite(const std::string& textureName);
    // Constructor for a sprite from a sub-rectangle of a texture
    Sprite(const std::string& textureName, const IntRect& textureRect);
    ~Sprite();

    void StartGraphics();
    void KillGraphics();

    // Set the source rectangle for the sprite from its texture
    void SetTextureRect(const IntRect& rect);

    // Set the scale of the sprite
    void SetScale(float x, float y);
    void SetScale(const glm::vec2& scale);

    void Draw(OurShader* shader, const glm::mat4& viewProjMat, float x, float y);

private:
    void UpdateGeometry();

    std::string mTextureName;
    IntRect mTextureRect;
    bool mUsesSubRect; // True if mTextureRect should be used
    glm::vec2 mScale;

    Texture* mTexture;
    SimpleGeom* mGeom;
};

#endif //HELLOC_SPRITE_HPP
