#ifndef HELLOC_PLAY_SCENE_HPP
#define HELLOC_PLAY_SCENE_HPP

#include "scene.hpp"
#include "../world/tilemap.hpp"
#include "../world/tileset.hpp"
#include "../core/sprite.hpp"
#include "../core/our_shader.hpp"
#include "glm/glm.hpp"
#include <memory>

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
    // Replace the old test sprite with our new map objects
    std::shared_ptr<TileSet> mTileSet;
    std::unique_ptr<Tilemap> mTileMap;

    glm::mat4 mViewProjMat;
};

#endif //HELLOC_PLAY_SCENE_HPP
