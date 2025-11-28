#ifndef HELLOC_PLAY_SCENE_HPP
#define HELLOC_PLAY_SCENE_HPP

#include "../scenes/scene.hpp"

class PlayScene : public Scene {
public:
    PlayScene();
    ~PlayScene() override;

    void DoFrame() override;
};

#endif //HELLOC_PLAY_SCENE_HPP
