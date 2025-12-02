#include "play_scene.hpp"
#include "../common.hpp"
#include "../core/scene_manager.hpp"
#include "../core/sfxman.hpp"
#include "glm/gtc/matrix_transform.hpp"

PlayScene::PlayScene() {
    mOurShader = nullptr;
    mTileSet = nullptr;
    mTileMap = nullptr;
    mTestSprite = nullptr;
}

PlayScene::~PlayScene() {
    if (mTestSprite) {
        delete mTestSprite;
    }
    if (mOurShader) {
        delete mOurShader;
    }
}

void PlayScene::OnStartGraphics() {
    mOurShader = new OurShader();
    mOurShader->Compile();

    mTestSprite = new Sprite(mOurShader, "my_logo");
    mTestSprite->StartGraphics();
    mTestSprite->SetTextureRect(IntRect{0, 0, 120, 120});
    mTestSprite->SetPosition(100, 100);
    // 1. Create the TileSet with the correct texture and data file paths from your assets
    mTileSet = std::make_shared<TileSet>("tileset1", "tileset1");

    // 2. Create the Tilemap, passing it the tileset
    mTileMap = std::make_unique<Tilemap>("tilemap1", mTileSet);
    // 3. Load all content. The tilemap will load the tileset, which loads the texture.
    mTileMap->LoadContent(mOurShader);

    UpdateViewProjMatrix();
}

void PlayScene::OnKillGraphics() {
    if (mTestSprite) {
        mTestSprite->KillGraphics();
        delete mTestSprite;
        mTestSprite = nullptr;
    }
    if (mOurShader) {
        delete mOurShader;
        mOurShader = nullptr;
    }


}

void PlayScene::OnScreenResized(int width, int height) {
    UpdateViewProjMatrix();
}

void PlayScene::UpdateViewProjMatrix() {
    float screenWidth = (float)SceneManager::GetInstance()->GetScreenWidth();
    float screenHeight = (float)SceneManager::GetInstance()->GetScreenHeight();
    mViewProjMat = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
}

void PlayScene::DoFrame() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mTileMap) {
        // For 2D rendering, we disable the depth test and rely on the painter's algorithm
        glDisable(GL_DEPTH_TEST);
        mTileMap->Render(mViewProjMat);
        glEnable(GL_DEPTH_TEST);
    }
}

void PlayScene::OnKeyDown(int ourKeyCode) { /* Do nothing for now */ }
void PlayScene::OnPointerDown(int pointerId, const struct PointerCoords *coords) {
    //SfxMan::GetInstance()->PlaySfx("sounds/test_sound.wav");

}
void PlayScene::OnPause() { /* Do nothing for now */ }
void PlayScene::OnResume() { /* Do nothing for now */ }

void PlayScene::OnInstall() {

    SfxMan::GetInstance()->LoadMusic("test_music");
    SfxMan::GetInstance()->StartMusic();
    // SfxMan::GetInstance()->preloadSound("sounds/test_sound.wav");

}

void PlayScene::OnUninstall() {
    SfxMan::GetInstance()->UnloadMusic();
    //  SfxMan::GetInstance()->unloadSound("sounds/test_sound.wav");
}
