#ifndef HELLOC_TITLE_SCREEN_SCENE_HPP
#define HELLOC_TITLE_SCREEN_SCENE_HPP

#include "scene.hpp"
#include "core/tex_quad.hpp"

class TitleScreenScene : public Scene {
public:
    TitleScreenScene();
    ~TitleScreenScene() override;

//    void OnInstall() override;
//    void OnUninstall() override;
//
//    void OnStartGraphics() override;
//    void OnKillGraphics() override;

    void DoFrame() override;

   // void OnPointerDown(int pointerId, const struct PointerCoords* coords) override;
   //void OnKeyDown(int ourKeycode) override;

private:
    TexQuad* mLogoQuad;
    float mAge;
    bool mFadingOut;
    bool mTransitionStarted;

    void TransitionToTitle();
};

#endif //HELLOC_TITLE_SCREEN_SCENE_HPP
