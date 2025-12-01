#ifndef HELLOC_PLAY_SCENE_HPP
#define HELLOC_PLAY_SCENE_HPP

#include "scene.hpp"
#include "../core/sprite.hpp"
#include "../core/our_shader.hpp"
#include "glm/glm.hpp"

class PlayScene : public Scene {
public:
    PlayScene();
    ~PlayScene() override;

    void OnStartGraphics() override;
    void OnKillGraphics() override;
    void OnScreenResized(int width, int height) override;
    void DoFrame() override;

    // Input and lifecycle events
    void OnKeyDown(int ourKeyCode) override;
    void OnPointerDown(int pointerId, const struct PointerCoords *coords) override;
    void OnPause() override;
    void OnResume() override;

private:
    void UpdateViewProjMatrix();

    OurShader* mOurShader;
    Sprite* mTestSprite;
    glm::mat4 mViewProjMat;
};

#endif //HELLOC_PLAY_SCENE_HPP
