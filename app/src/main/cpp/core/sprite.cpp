#include "sprite.hpp"
#include "our_shader.hpp"
#include "simplegeom.hpp"
#include "texture.hpp"
#include "vertexbuf.hpp"
#include "common.hpp"

Sprite::Sprite(const std::string& textureName, const IntRect& textureRect) :
    mTextureName(textureName), mTextureRect(textureRect), mTexture(nullptr), mGeom(nullptr) {
}

Sprite::~Sprite() {
    KillGraphics();
}

void Sprite::StartGraphics() {
    mTexture = new Texture();
    mTexture->InitFromAsset(mTextureName.c_str());

    float u0 = static_cast<float>(mTextureRect.left) / mTexture->GetWidth();
    float v0 = static_cast<float>(mTextureRect.top) / mTexture->GetHeight();
    float u1 = static_cast<float>(mTextureRect.left + mTextureRect.width) / mTexture->GetWidth();
    float v1 = static_cast<float>(mTextureRect.top + mTextureRect.height) / mTexture->GetHeight();

    // Define vertices with position, color, and texture coordinates
    GLfloat vertices[] = {
        // positions          // colors           // texture coords
        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u0, v1,
         0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u1, v1,
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u0, v0,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   u1, v0,
    };

    // Use the new, correct constructor
    VertexBuf* vbuf = new VertexBuf(vertices, sizeof(vertices), 8 * sizeof(GLfloat), 3 * sizeof(GLfloat), 6 * sizeof(GLfloat));
    vbuf->SetPrimitive(GL_TRIANGLE_STRIP);

    mGeom = new SimpleGeom(vbuf);
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

void Sprite::Draw(OurShader* shader, float x, float y) {
    if (!mGeom || !mTexture || !shader) {
        return;
    }

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    glm::mat4 mvpMat = modelMat; // Simplified MVP matrix

    // The correct rendering sequence:
    shader->BeginRender(mGeom->vbuf);
    shader->SetTexture(mTexture);
    shader->Render(&mvpMat);
    shader->EndRender();
}
