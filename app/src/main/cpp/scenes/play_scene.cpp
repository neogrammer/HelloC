#include "play_scene.hpp"
#include "../common.hpp"

PlayScene::PlayScene() {
    mOurShader = nullptr;
    mTestSprite = nullptr;
}

PlayScene::~PlayScene() {
    // Note: OnKillGraphics is responsible for de-allocating graphics resources.
    // This destructor will be called when the scene is no longer needed.
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

    IntRect textureRect = {0, 0, 256, 256};
    mTestSprite = new Sprite("textures/my_logo.png", textureRect);
    mTestSprite->StartGraphics();
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
    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mTestSprite && mOurShader) {
        mTestSprite->Draw(mOurShader, 0.0f, 0.0f);
    }
}
