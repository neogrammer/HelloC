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
    Sprite(OurShader* shader, const std::string& textureName);
    Sprite(OurShader* shader, const std::string& textureName, const IntRect& textureRect);
    ~Sprite();

    void StartGraphics();
    void KillGraphics();

    void SetTextureRect(const struct IntRect& rect);
    void SetScale(float x, float y);
    void SetScale(const glm::vec2& scale);
    void SetPosition(float x, float y);
    void SetPosition(const glm::vec2& pos);

    // The clean draw call we want!
    void Draw(const glm::mat4& viewProjMat);

private:
    void UpdateGeometry();

    // Rendering state
    OurShader* mShader; // Non-owning pointer
    Texture* mTexture;
    SimpleGeom* mGeom;

    // Sprite properties
    std::string mTextureName;
    IntRect mTextureRect;
    bool mUsesSubRect;
    glm::vec2 mScale;
    glm::vec2 mPosition;
};

#endif //HELLOC_SPRITE_HPP
