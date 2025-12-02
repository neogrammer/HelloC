#include "sprite.hpp"
#include "our_shader.hpp"
#include "simplegeom.hpp"
#include "texture.hpp"
#include "vertexbuf.hpp"
#include "common.hpp"
#include "int_rect.hpp"
Sprite::Sprite(OurShader* shader, const std::string& textureName)
    : mShader(shader),
      mTexture(nullptr),
      mGeom(nullptr),
      mTextureName(textureName),
      mTextureRect(IntRect{0,0,0,0}),
      mUsesSubRect(false),
      mScale(1.0f, 1.0f),
      mPosition(0.0f, 0.0f)
      {}

Sprite::Sprite(OurShader* shader, const std::string& textureName, const IntRect& textureRect)
    : mShader(shader),
      mTexture(nullptr),
      mGeom(nullptr),
      mTextureName(textureName),
      mTextureRect(textureRect),
      mUsesSubRect(true),
      mScale(1.0f, 1.0f),
      mPosition(0.0f, 0.0f)
      {}

Sprite::~Sprite() {
    KillGraphics();
}

void Sprite::StartGraphics() {
    mTexture = new Texture();
    mTexture->InitFromAsset(mTextureName.c_str());
    UpdateGeometry();
}

void Sprite::KillGraphics() {
    if (mTexture) {
        delete mTexture;
        mTexture = nullptr;
    }
    if (mGeom) {
        delete mGeom;
        mGeom = nullptr;
    }
}

void Sprite::SetTextureRect(const IntRect& rect) {
    mTextureRect = rect;
    mUsesSubRect = true;
    UpdateGeometry();
}

void Sprite::SetScale(float x, float y) {
    mScale = glm::vec2(x, y);
}

void Sprite::SetScale(const glm::vec2& scale) {
    mScale = scale;
}

void Sprite::SetPosition(float x, float y) {
    mPosition = glm::vec2(x, y);
}

void Sprite::SetPosition(const glm::vec2& pos) {
    mPosition = pos;
}

void Sprite::UpdateGeometry() {
    if (!mTexture || mTexture->GetWidth() == 0 || mTexture->GetHeight() == 0) {
        return;
    }

    if (mGeom) {
        delete mGeom;
        mGeom = nullptr;
    }

    IntRect sourceRect = mUsesSubRect ? mTextureRect : IntRect{0, 0, mTexture->GetWidth(), mTexture->GetHeight()};

    float u0 = static_cast<float>(sourceRect.left) / mTexture->GetWidth();
    float v0 = static_cast<float>(sourceRect.top) / mTexture->GetHeight();
    float u1 = static_cast<float>(sourceRect.left + sourceRect.width) / mTexture->GetWidth();
    float v1 = static_cast<float>(sourceRect.top + sourceRect.height) / mTexture->GetHeight();

//    GLfloat vertices[] = {
//        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u0, v1,
//         0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u1, v1,
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u0, v0,
//         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u1, v0,
//    };

    GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u0, v0,
            0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u1, v0,
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u0, v1,
            0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u1, v1,
    };

    VertexBuf* vbuf = new VertexBuf(vertices, sizeof(vertices), 8 * sizeof(GLfloat), 3 * sizeof(GLfloat), 6 * sizeof(GLfloat));
    vbuf->SetPrimitive(GL_TRIANGLE_STRIP);

    mGeom = new SimpleGeom(vbuf);
}

void Sprite::Draw(const glm::mat4& viewProjMat) {
    if (!mGeom || !mTexture || !mShader) {
        return;
    }

    float spriteWidth = mUsesSubRect ? mTextureRect.width : mTexture->GetWidth();
    float spriteHeight = mUsesSubRect ? mTextureRect.height : mTexture->GetHeight();

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(mPosition.x, mPosition.y, 0.0f));
    modelMat = glm::scale(modelMat, glm::vec3(spriteWidth * mScale.x, spriteHeight * mScale.y, 1.0f));

    glm::mat4 mvpMat = viewProjMat * modelMat;

    mShader->BeginRender(mGeom->vbuf);
    mShader->SetTexture(mTexture);
    mShader->Render(&mvpMat);
    mShader->EndRender();
}
