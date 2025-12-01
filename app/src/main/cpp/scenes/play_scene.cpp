#include "play_scene.hpp"
#include "../common.hpp"
#include "../core/scene_manager.hpp"
#include "../core/sfxman.hpp"
#include "glm/gtc/matrix_transform.hpp"

PlayScene::PlayScene() {
    mOurShader = nullptr;
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

    mTestSprite = new Sprite(mOurShader, "textures/my_logo.png");
    mTestSprite->StartGraphics();
    mTestSprite->SetTextureRect(IntRect{0, 0, 120, 120});
    mTestSprite->SetPosition(100, 100);

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
    SceneManager* sm = SceneManager::GetInstance();
    mViewProjMat = glm::ortho(0.0f, (float)sm->GetScreenWidth(), (float)sm->GetScreenHeight(), 0.0f, -1.0f, 1.0f);
}

void PlayScene::DoFrame() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mTestSprite) {
        mTestSprite->Draw(mViewProjMat);
    }
}

void PlayScene::OnKeyDown(int ourKeyCode) {
    // Handle key input here
}



void PlayScene::OnPointerDown(int pointerId, const struct PointerCoords *coords) {
    // Play a tone when the screen is touched
    SfxMan::GetInstance()->PlayTone("d100 f440.");
}

void PlayScene::OnPause() {
    // Handle game pausing here
}

void PlayScene::OnResume() {
    // Handle game resuming here
}
