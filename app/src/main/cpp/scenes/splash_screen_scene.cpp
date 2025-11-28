#include "splash_screen_scene.hpp"
#include "title_screen_scene.hpp"
#include "../common.hpp"
#include "../core/scene_manager.hpp"
#include "../core/util.hpp"
#include "../core/vertexbuf.hpp"
#include "../core/indexbuf.hpp"
#include "glm/glm.hpp"

// 5 seconds for fade in, 5 for fade out.
const float FADE_TIME = 5.0f;

// vertices for a centered quad, 1/5th of screen width and 1/3rd of screen height
// Texture coordinates are flipped on the Y axis to fix upside-down rendering.
static const GLfloat QUAD_VERTICES[] = {
    // Position (1/5 width, 1/3 height)  // Tex Coords (Y-flipped)  // Color (white)
    -0.2f, -1.0f/3.0f, 0.0f,             0.0f, 1.0f,                  1.0f, 1.0f, 1.0f,
     0.2f, -1.0f/3.0f, 0.0f,             1.0f, 1.0f,                  1.0f, 1.0f, 1.0f,
    -0.2f,  1.0f/3.0f, 0.0f,             0.0f, 0.0f,                  1.0f, 1.0f, 1.0f,
     0.2f,  1.0f/3.0f, 0.0f,             1.0f, 0.0f,                  1.0f, 1.0f, 1.0f,
};

static const GLushort QUAD_INDICES[] = {
    0, 1, 2, 2, 1, 3
};

SplashScreenScene::SplashScreenScene() : mClock() {
    mLogoGeom = nullptr;
    mLogoTexture = nullptr;
    mShader = nullptr;
    mAge = 0.0f;
    mFadingOut = false;
    mTransitionStarted = false;
}

SplashScreenScene::~SplashScreenScene() {
    CleanUp(&mLogoGeom);
    CleanUp(&mLogoTexture);
    CleanUp(&mShader);
}

void SplashScreenScene::OnInstall() {
    mAge = 0.0f;
    mFadingOut = false;
    mTransitionStarted = false;
    mClock.Reset();
}

void SplashScreenScene::OnUninstall() {
}

void SplashScreenScene::OnStartGraphics() {
    // Create the geometry for the logo quad
    VertexBuf* vbuf = new VertexBuf(const_cast<GLfloat *>(QUAD_VERTICES), sizeof(QUAD_VERTICES), 8 * sizeof(GLfloat));
    vbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    vbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* ibuf = new IndexBuf(const_cast<GLushort *>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mLogoGeom = new SimpleGeom(vbuf, ibuf);

    // Create the texture for the logo
    mLogoTexture = new Texture();
    mLogoTexture->InitFromAsset("textures/my_logo.png");

    // Create a shader
    mShader = new OurShader();
    mShader->Compile();
}

void SplashScreenScene::OnKillGraphics() {
    CleanUp(&mLogoGeom);
    CleanUp(&mLogoTexture);
    CleanUp(&mShader);
}

void SplashScreenScene::DoFrame() {
    // if we're already transitioning, do nothing.
    if (mTransitionStarted) {
        return;
    }

    // update age
    mAge += mClock.ReadDelta();

    // determine alpha value
    float alpha = 1.0f;
    if (mFadingOut) {
        if (mAge >= FADE_TIME) {
            // Faded out. Time to transition.
            TransitionToTitle();
            return;
        }
        alpha = 1.0f - (mAge / FADE_TIME);
    } else {
        if (mAge >= FADE_TIME) {
            // Faded in. Switch to fading out.
            mFadingOut = true;
            mAge = 0.0f;
            mClock.Reset();
            // alpha is 1.0, which is correct.
        } else {
            alpha = mAge / FADE_TIME;
        }
    }

    // Render the logo quad.
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShader->BeginRender(mLogoGeom->vbuf);
    mShader->SetTintColor(1.0f, 1.0f, 1.0f, alpha);
    mShader->SetTexture(mLogoTexture);
    glm::mat4 identityMat(1.0f);
    mShader->Render(mLogoGeom->ibuf, &identityMat);
    mShader->EndRender();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void SplashScreenScene::OnPointerDown(int pointerId, const struct PointerCoords* coords) {
    // Any touch skips the splash screen
    TransitionToTitle();
}

void SplashScreenScene::OnKeyDown(int ourKeycode) {
    // Any key press skips the splash screen
    TransitionToTitle();
}

void SplashScreenScene::TransitionToTitle() {
    if (!mTransitionStarted) {
        mTransitionStarted = true;
        SceneManager::GetInstance()->RequestNewScene(new TitleScreenScene());
    }
}
