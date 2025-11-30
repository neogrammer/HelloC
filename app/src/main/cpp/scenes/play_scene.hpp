#ifndef HELLOC_PLAY_SCENE_HPP
#define HELLOC_PLAY_SCENE_HPP

#include "scene.hpp"
#include "../core/sprite.hpp"
#include "../core/our_shader.hpp"

class PlayScene : public Scene {
public:
    PlayScene();
    ~PlayScene() override;

    void OnStartGraphics() override;
    void OnKillGraphics() override;
    void DoFrame() override;

private:
    OurShader* mOurShader;
    Sprite* mTestSprite;
};

#endif //HELLOC_PLAY_SCENE_HPP
