#include "title_screen_scene.hpp"
#include "play_scene.hpp"
#include "../common.hpp"
#include "../core/scene_manager.hpp"
#include "../core/util.hpp"
#include "../core/vertexbuf.hpp"
#include "../core/indexbuf.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "sfxman.hpp"

// 5 seconds for fade in
const float TITLE_FADE_TIME = 2.0f;

// Bounding boxes for buttons (in normalized screen coordinates)
const float PLAY_BUTTON_LEFT = 0.6f;
const float PLAY_BUTTON_RIGHT = 0.9f;
const float PLAY_BUTTON_TOP = 0.6f;
const float PLAY_BUTTON_BOTTOM = 0.4f;

const float OPTIONS_BUTTON_LEFT = 0.6f;
const float OPTIONS_BUTTON_RIGHT = 0.9f;
const float OPTIONS_BUTTON_TOP = 0.3f;
const float OPTIONS_BUTTON_BOTTOM = 0.1f;


// vertices for a 3/4 width background quad
static const GLfloat BG_VERTICES[] = {
    // Position           // Tex Coords   // Color
    -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,     1.0f, 1.0f, 1.0f, // bottom-left
     0.5f, -1.0f, 0.0f,   1.0f, 1.0f,     1.0f, 1.0f, 1.0f, // bottom-right
    -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,     1.0f, 1.0f, 1.0f, // top-left
     0.5f,  1.0f, 0.0f,   1.0f, 0.0f,     1.0f, 1.0f, 1.0f  // top-right
};

static const GLushort QUAD_INDICES[] = { 0, 1, 2, 2, 1, 3 };

// Vertices for the title quad
static const GLfloat TITLE_VERTICES[] = {
    // Position           // Tex Coords   // Color
     0.6f,  0.7f, 0.0f,    0.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     0.9f,  0.7f, 0.0f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     0.6f,  0.9f, 0.0f,    0.0f, 0.0f,     1.0f, 1.0f, 1.0f,
     0.9f,  0.9f, 0.0f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
};

// Vertices for the play button quad
static const GLfloat PLAY_BUTTON_VERTICES[] = {
    // Position           // Tex Coords   // Color
     PLAY_BUTTON_LEFT,  PLAY_BUTTON_BOTTOM, 0.0f,    0.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     PLAY_BUTTON_RIGHT, PLAY_BUTTON_BOTTOM, 0.0f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     PLAY_BUTTON_LEFT,  PLAY_BUTTON_TOP, 0.0f,    0.0f, 0.0f,     1.0f, 1.0f, 1.0f,
     PLAY_BUTTON_RIGHT, PLAY_BUTTON_TOP, 0.0f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
};

// Vertices for the options button quad
static const GLfloat OPTIONS_BUTTON_VERTICES[] = {
    // Position           // Tex Coords   // Color
     OPTIONS_BUTTON_LEFT,  OPTIONS_BUTTON_BOTTOM, 0.0f,    0.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     OPTIONS_BUTTON_RIGHT, OPTIONS_BUTTON_BOTTOM, 0.0f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     OPTIONS_BUTTON_LEFT,  OPTIONS_BUTTON_TOP, 0.0f,    0.0f, 0.0f,     1.0f, 1.0f, 1.0f,
     OPTIONS_BUTTON_RIGHT, OPTIONS_BUTTON_TOP, 0.0f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
};

TitleScreenScene::TitleScreenScene() : mClock() {
    mAge = 0.0f;
    mSkipFade = false;
    mBgGeom = nullptr;
    mBgTexture = nullptr;
    mTitleGeom = nullptr;
    mTitleTexture = nullptr;
    mPlayButtonGeom = nullptr;
    mPlayButtonTexture = nullptr;
    mOptionsButtonGeom = nullptr;
    mOptionsButtonTexture = nullptr;
    mShader = nullptr;
}

TitleScreenScene::~TitleScreenScene() {
    CleanUp(&mBgGeom);
    CleanUp(&mBgTexture);
    CleanUp(&mTitleGeom);
    CleanUp(&mTitleTexture);
    CleanUp(&mPlayButtonGeom);
    CleanUp(&mPlayButtonTexture);
    CleanUp(&mOptionsButtonGeom);
    CleanUp(&mOptionsButtonTexture);
    CleanUp(&mShader);
}

void TitleScreenScene::OnStartGraphics() {
    // background quad
    VertexBuf* bg_vbuf = new VertexBuf(const_cast<GLfloat*>(BG_VERTICES), sizeof(BG_VERTICES), 8 * sizeof(GLfloat));
    bg_vbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    bg_vbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* bg_ibuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mBgGeom = new SimpleGeom(bg_vbuf, bg_ibuf);

    mBgTexture = new Texture();
    mBgTexture->InitFromAsset("titleBG");

    // title quad
    VertexBuf* title_vbuf = new VertexBuf(const_cast<GLfloat*>(TITLE_VERTICES), sizeof(TITLE_VERTICES), 8 * sizeof(GLfloat));
    title_vbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    title_vbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* title_ibuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mTitleGeom = new SimpleGeom(title_vbuf, title_ibuf);

    mTitleTexture = new Texture();
    mTitleTexture->InitFromAsset("title_text");

    // play button quad
    VertexBuf* play_vbuf = new VertexBuf(const_cast<GLfloat*>(PLAY_BUTTON_VERTICES), sizeof(PLAY_BUTTON_VERTICES), 8 * sizeof(GLfloat));
    play_vbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    play_vbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* play_ibuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mPlayButtonGeom = new SimpleGeom(play_vbuf, play_ibuf);

    mPlayButtonTexture = new Texture();
    mPlayButtonTexture->InitFromAsset("play_button");

    // options button quad
    VertexBuf* options_vbuf = new VertexBuf(const_cast<GLfloat*>(OPTIONS_BUTTON_VERTICES), sizeof(OPTIONS_BUTTON_VERTICES), 8 * sizeof(GLfloat));
    options_vbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    options_vbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* options_ibuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mOptionsButtonGeom = new SimpleGeom(options_vbuf, options_ibuf);

    mOptionsButtonTexture = new Texture();
    mOptionsButtonTexture->InitFromAsset("options_button");

    mShader = new OurShader();
    mShader->Compile();
}

void TitleScreenScene::OnKillGraphics() {
    CleanUp(&mBgGeom);
    CleanUp(&mBgTexture);
    CleanUp(&mTitleGeom);
    CleanUp(&mTitleTexture);
    CleanUp(&mPlayButtonGeom);
    CleanUp(&mPlayButtonTexture);
    CleanUp(&mOptionsButtonGeom);
    CleanUp(&mOptionsButtonTexture);
    CleanUp(&mShader);
}

void TitleScreenScene::DoFrame() {
    // update age
    mAge += mClock.ReadDelta();

    // determine alpha value
    float alpha = 1.0f;
    if (!mSkipFade && mAge < TITLE_FADE_TIME) {
        alpha = mAge / TITLE_FADE_TIME;
    }

    // Render
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // white background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 identityMat(1.0f);

    // Render the background
    mShader->BeginRender(mBgGeom->vbuf);
    mShader->SetTintColor(1.0f, 1.0f, 1.0f, alpha);
    mShader->SetTexture(mBgTexture);
    mShader->Render(mBgGeom->ibuf, &identityMat);
    mShader->EndRender();

    // Render the title
    mShader->BeginRender(mTitleGeom->vbuf);
    mShader->SetTintColor(1.0f, 1.0f, 1.0f, alpha);
    mShader->SetTexture(mTitleTexture);
    mShader->Render(mTitleGeom->ibuf, &identityMat);
    mShader->EndRender();

    // Render the buttons
    mShader->BeginRender(mPlayButtonGeom->vbuf);
    mShader->SetTintColor(1.0f, 1.0f, 1.0f, alpha);
    mShader->SetTexture(mPlayButtonTexture);
    mShader->Render(mPlayButtonGeom->ibuf, &identityMat);
    mShader->EndRender();

    mShader->BeginRender(mOptionsButtonGeom->vbuf);
    mShader->SetTintColor(1.0f, 1.0f, 1.0f, alpha);
    mShader->SetTexture(mOptionsButtonTexture);
    mShader->Render(mOptionsButtonGeom->ibuf, &identityMat);
    mShader->EndRender();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void TitleScreenScene::OnPointerDown(int pointerId, const struct PointerCoords* coords) {
    // convert to normalized screen coordinates
    float screenWidth = SceneManager::GetInstance()->GetScreenWidth();
    float screenHeight = SceneManager::GetInstance()->GetScreenHeight();
    float touchX = (coords->x / screenWidth) * 2.0f - 1.0f;
    float touchY = ((screenHeight - coords->y) / screenHeight) * 2.0f - 1.0f;

    if (touchX >= PLAY_BUTTON_LEFT && touchX <= PLAY_BUTTON_RIGHT &&
        touchY >= PLAY_BUTTON_BOTTOM && touchY <= PLAY_BUTTON_TOP) {
        SceneManager::GetInstance()->RequestNewScene(new PlayScene());
    } else if (touchX >= OPTIONS_BUTTON_LEFT && touchX <= OPTIONS_BUTTON_RIGHT &&
               touchY >= OPTIONS_BUTTON_BOTTOM && touchY <= OPTIONS_BUTTON_TOP) {
        LOGD("Options button clicked!");
    }

    SfxMan::GetInstance()->PlaySfx("test_sound");
}

void TitleScreenScene::OnInstall() {
   // SfxMan::GetInstance()->LoadMusic("test_music");
  //  SfxMan::GetInstance()->StartMusic();
  SfxMan::GetInstance()->preloadSound("test_sound");

}

void TitleScreenScene::OnUninstall() {
  //  SfxMan::GetInstance()->UnloadMusic();
  SfxMan::GetInstance()->unloadSound("test_sound");
}
