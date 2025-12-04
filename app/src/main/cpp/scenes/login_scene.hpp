#ifndef HELLOC_LOGIN_SCENE_HPP
#define HELLOC_LOGIN_SCENE_HPP

#include "scene.hpp"
#include "../core/simplegeom.hpp"
#include "../core/texture.hpp"
#include "../core/our_shader.hpp"
#include "../core/util.hpp"
#include "../core/native_engine.hpp"
#include "../third_party/auth/auth_service.grpc.pb.h"

class LoginScene : public Scene {
public:
    LoginScene();
    ~LoginScene() override;

    void OnStartGraphics() override;
    void OnKillGraphics() override;
    void DoFrame() override;
    void OnPointerDown(int pointerId, const struct PointerCoords* coords) override;
    void OnInstall() override;
    void OnUninstall() override;

private:
    DeltaClock mClock;

    // UI elements
    SimpleGeom* mSignInButtonGeom;
    Texture* mSignInButtonTexture;
    SimpleGeom* mSignUpButtonGeom;
    Texture* mSignUpButtonTexture;
    SimpleGeom* mBackButtonGeom;
    Texture* mBackButtonTexture;

    OurShader* mShader;
};

#endif //HELLOC_LOGIN_SCENE_HPP
