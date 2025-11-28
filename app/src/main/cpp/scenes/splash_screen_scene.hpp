#ifndef HELLOC_SPLASH_SCREEN_SCENE_HPP
#define HELLOC_SPLASH_SCREEN_SCENE_HPP

#include "../scenes/scene.hpp"
#include "../core/our_shader.hpp"
#include "../core/simplegeom.hpp"
#include "../core/texture.hpp"
#include "../core/util.hpp"
class OurShader;
class SplashScreenScene : public Scene {
public:
    SplashScreenScene();
    ~SplashScreenScene() override;

    void OnInstall() override;
    void OnUninstall() override;

    void OnStartGraphics() override;
    void OnKillGraphics() override;

    void DoFrame() override;

    void OnPointerDown(int pointerId, const struct PointerCoords* coords) override;
    void OnKeyDown(int ourKeycode) override;

private:
    void TransitionToTitle();

    SimpleGeom* mLogoGeom;
    Texture* mLogoTexture;
    OurShader* mShader;

    DeltaClock mClock;
    float mAge;
    bool mFadingOut;
    bool mTransitionStarted;
};

#endif //HELLOC_SPLASH_SCREEN_SCENE_HPP
