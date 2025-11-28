#ifndef HELLOC_TITLE_SCREEN_SCENE_HPP
#define HELLOC_TITLE_SCREEN_SCENE_HPP

#include "../scenes/scene.hpp"
#include "../core/simplegeom.hpp"
#include "../core/texture.hpp"
#include "../core/our_shader.hpp"
#include "../core/util.hpp"

class TitleScreenScene : public Scene {
public:
    TitleScreenScene();
    ~TitleScreenScene() override;

    void OnStartGraphics() override;
    void OnKillGraphics() override;
    void DoFrame() override;
    void OnPointerDown(int pointerId, const struct PointerCoords* coords) override;

private:
    float mAge;
    DeltaClock mClock;
    bool mSkipFade;

    // background
    SimpleGeom* mBgGeom;
    Texture* mBgTexture;

    // title text
    SimpleGeom* mTitleGeom;
    Texture* mTitleTexture;

    // buttons
    SimpleGeom* mPlayButtonGeom;
    Texture* mPlayButtonTexture;
    SimpleGeom* mOptionsButtonGeom;
    Texture* mOptionsButtonTexture;

    OurShader* mShader;
};

#endif //HELLOC_TITLE_SCREEN_SCENE_HPP
