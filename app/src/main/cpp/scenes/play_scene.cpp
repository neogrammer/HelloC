#include "play_scene.hpp"
#include "../common.hpp"
#include "../core/scene_manager.hpp"
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

    mTestSprite = new Sprite("textures/my_logo.png");
    mTestSprite->StartGraphics();
    mTestSprite->SetTextureRect(IntRect{0, 0, 120, 120});
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

void PlayScene::DoFrame() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SceneManager* sm = SceneManager::GetInstance();
    glm::mat4 viewProjMat = glm::ortho(0.0f, (float)sm->GetScreenWidth(), (float)sm->GetScreenHeight(), 0.0f, -1.0f, 1.0f);

    if (mTestSprite && mOurShader) {
        mTestSprite->Draw(mOurShader, viewProjMat, 100.0f, 100.0f);
    }
}
