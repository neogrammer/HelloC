#ifndef HELLOC_OUR_SHADER_HPP
#define HELLOC_OUR_SHADER_HPP

#include "shader.hpp"
#include "texture.hpp"

// A shader that can render geometry with color and texture.
// This is the "main" shader in the game.
class OurShader : public Shader {
private:
    GLint mColorLoc;
    GLint mTintLoc;
    GLint mSamplerLoc;
    GLint mTexCoordLoc;
    GLint mPointLightPosLoc;
    GLint mPointLightColorLoc;

public:
    OurShader();
    ~OurShader() override;
    void Compile() override;

    // Sets the tint color. This is multiplied by the vertex color and texture color.
    void SetTintColor(float r, float g, float b, float a);

    // Sets the texture.
    void SetTexture(Texture* t);

    // Enables a point light.
    void EnablePointLight(glm::vec3 pos, float r, float g, float b);

    // Disables the point light.
    void DisablePointLight();

    void BeginRender(VertexBuf* geom) override;

protected:
    const char* GetVertShaderSource() override;
    const char* GetFragShaderSource() override;
    const char* GetShaderName() override;
};

#endif //HELLOC_OUR_SHADER_HPP
