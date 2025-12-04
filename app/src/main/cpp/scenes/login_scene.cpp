#include "login_scene.hpp"
#include "title_screen_scene.hpp"
#include "play_scene.hpp"
#include "../common.hpp"
#include "../core/scene_manager.hpp"
#include "../core/util.hpp"
#include "../core/vertexbuf.hpp"
#include "../core/indexbuf.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <grpc++/grpc++.h>

// Bounding boxes for buttons
const float SIGN_IN_BUTTON_LEFT = 0.2f;
const float SIGN_IN_BUTTON_RIGHT = 0.8f;
const float SIGN_IN_BUTTON_TOP = 0.6f;
const float SIGN_IN_BUTTON_BOTTOM = 0.4f;

const float SIGN_UP_BUTTON_LEFT = 0.2f;
const float SIGN_UP_BUTTON_RIGHT = 0.8f;
const float SIGN_UP_BUTTON_TOP = 0.3f;
const float SIGN_UP_BUTTON_BOTTOM = 0.1f;

const float BACK_BUTTON_LEFT = 0.1f;
const float BACK_BUTTON_RIGHT = 0.3f;
const float BACK_BUTTON_TOP = 0.9f;
const float BACK_BUTTON_BOTTOM = 0.7f;


// Vertices for the button quads
static const GLfloat BUTTON_VERTICES[] = {
    // Position           // Tex Coords   // Color
    -0.3f, -0.1f, 0.0f,   0.0f, 1.0f,     1.0f, 1.0f, 1.0f,
     0.3f, -0.1f, 0.0f,   1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
    -0.3f,  0.1f, 0.0f,   0.0f, 0.0f,     1.0f, 1.0f, 1.0f,
     0.3f,  0.1f, 0.0f,   1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
};

static const GLushort QUAD_INDICES[] = { 0, 1, 2, 2, 1, 3 };

LoginScene::LoginScene() : mClock() {
    mSignInButtonGeom = nullptr;
    mSignInButtonTexture = nullptr;
    mSignUpButtonGeom = nullptr;
    mSignUpButtonTexture = nullptr;
    mBackButtonGeom = nullptr;
    mBackButtonTexture = nullptr;
    mShader = nullptr;
}

LoginScene::~LoginScene() {
    CleanUp(&mSignInButtonGeom);
    CleanUp(&mSignInButtonTexture);
    CleanUp(&mSignUpButtonGeom);
    CleanUp(&mSignUpButtonTexture);
    CleanUp(&mBackButtonGeom);
    CleanUp(&mBackButtonTexture);
    CleanUp(&mShader);
}

void LoginScene::OnStartGraphics() {
    // Create a shader
    mShader = new OurShader();
    mShader->Compile();

    // Create button geometries
    VertexBuf* signInVbuf = new VertexBuf(const_cast<GLfloat*>(BUTTON_VERTICES), sizeof(BUTTON_VERTICES), 8 * sizeof(GLfloat));
    signInVbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    signInVbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* signInIbuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mSignInButtonGeom = new SimpleGeom(signInVbuf, signInIbuf);

    VertexBuf* signUpVbuf = new VertexBuf(const_cast<GLfloat*>(BUTTON_VERTICES), sizeof(BUTTON_VERTICES), 8 * sizeof(GLfloat));
    signUpVbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    signUpVbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* signUpIbuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mSignUpButtonGeom = new SimpleGeom(signUpVbuf, signUpIbuf);

    VertexBuf* backVbuf = new VertexBuf(const_cast<GLfloat*>(BUTTON_VERTICES), sizeof(BUTTON_VERTICES), 8 * sizeof(GLfloat));
    backVbuf->SetTexCoordsOffset(3 * sizeof(GLfloat));
    backVbuf->SetColorsOffset(5 * sizeof(GLfloat));
    IndexBuf* backIbuf = new IndexBuf(const_cast<GLushort*>(QUAD_INDICES), sizeof(QUAD_INDICES));
    mBackButtonGeom = new SimpleGeom(backVbuf, backIbuf);

    // Create button textures
    mSignInButtonTexture = new Texture();
    mSignInButtonTexture->InitFromAsset("play_button");

    mSignUpButtonTexture = new Texture();
    mSignUpButtonTexture->InitFromAsset("options_button");

    mBackButtonTexture = new Texture();
    mBackButtonTexture->InitFromAsset("options_button");
}

void LoginScene::OnKillGraphics() {
    CleanUp(&mSignInButtonGeom);
    CleanUp(&mSignInButtonTexture);
    CleanUp(&mSignUpButtonGeom);
    CleanUp(&mSignUpButtonTexture);
    CleanUp(&mBackButtonGeom);
    CleanUp(&mBackButtonTexture);
    CleanUp(&mShader);
}

void LoginScene::DoFrame() {
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f); // Blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 identityMat(1.0f);

    // Render buttons
    mShader->BeginRender(mSignInButtonGeom->vbuf);
    mShader->SetTexture(mSignInButtonTexture);
    glm::mat4 modelMat = glm::translate(identityMat, glm::vec3((SIGN_IN_BUTTON_LEFT + SIGN_IN_BUTTON_RIGHT) / 2.0f, (SIGN_IN_BUTTON_TOP + SIGN_IN_BUTTON_BOTTOM) / 2.0f, 0.0f));
    mShader->Render(mSignInButtonGeom->ibuf, &modelMat);
    mShader->EndRender();

    mShader->BeginRender(mSignUpButtonGeom->vbuf);
    mShader->SetTexture(mSignUpButtonTexture);
    modelMat = glm::translate(identityMat, glm::vec3((SIGN_UP_BUTTON_LEFT + SIGN_UP_BUTTON_RIGHT) / 2.0f, (SIGN_UP_BUTTON_TOP + SIGN_UP_BUTTON_BOTTOM) / 2.0f, 0.0f));
    mShader->Render(mSignUpButtonGeom->ibuf, &modelMat);
    mShader->EndRender();

    mShader->BeginRender(mBackButtonGeom->vbuf);
    mShader->SetTexture(mBackButtonTexture);
    modelMat = glm::translate(identityMat, glm::vec3((BACK_BUTTON_LEFT + BACK_BUTTON_RIGHT) / 2.0f, (BACK_BUTTON_TOP + BACK_BUTTON_BOTTOM) / 2.0f, 0.0f));
    mShader->Render(mBackButtonGeom->ibuf, &modelMat);
    mShader->EndRender();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void LoginScene::OnPointerDown(int pointerId, const struct PointerCoords* coords) {
    float screenWidth = SceneManager::GetInstance()->GetScreenWidth();
    float screenHeight = SceneManager::GetInstance()->GetScreenHeight();
    float touchX = (coords->x / screenWidth) * 2.0f - 1.0f;
    float touchY = ((screenHeight - coords->y) / screenHeight) * 2.0f - 1.0f;

    if (touchX >= SIGN_IN_BUTTON_LEFT && touchX <= SIGN_IN_BUTTON_RIGHT &&
        touchY >= SIGN_IN_BUTTON_BOTTOM && touchY <= SIGN_IN_BUTTON_TOP) {
        LOGD("Sign In button clicked!");
        std::shared_ptr<grpc::Channel> channel = NativeEngine::GetInstance()->GetChannel();
        std::unique_ptr<AuthService::Stub> stub = AuthService::NewStub(channel);

        LoginRequest request;
        request.set_email("test@grpc.com");
        request.set_password("a_strong_password");

        AuthResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub->Login(&context, request, &response);

        if (status.ok()) {
            LOGD("Login successful! Token: %s", response.session_token().c_str());
            SceneManager::GetInstance()->RequestNewScene(new PlayScene());
        } else {
            LOGE("Login failed: %s", status.error_message().c_str());
        }
    } else if (touchX >= SIGN_UP_BUTTON_LEFT && touchX <= SIGN_UP_BUTTON_RIGHT &&
               touchY >= SIGN_UP_BUTTON_BOTTOM && touchY <= SIGN_UP_BUTTON_TOP) {
        LOGD("Sign Up button clicked!");
        // TODO: Implement Sign Up logic
    } else if (touchX >= BACK_BUTTON_LEFT && touchX <= BACK_BUTTON_RIGHT &&
               touchY >= BACK_BUTTON_BOTTOM && touchY <= BACK_BUTTON_TOP) {
        LOGD("Back button clicked!");
        SceneManager::GetInstance()->RequestNewScene(new TitleScreenScene());
    }
}

void LoginScene::OnInstall() {
    // Nothing to do here for now
}

void LoginScene::OnUninstall() {
    // Nothing to do here for now
}
