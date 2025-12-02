//
// Created by jlhar on 11/27/2025.
//
#include "scene_manager.hpp"

#include "common.hpp"
#include "scenes/scene.hpp"
#include <android/input.h>

static SceneManager _sceneManager;

SceneManager::SceneManager() {
    mCurScene = NULL;

    // start with non-bogus (though not accurate) values
    mScreenWidth = 320;
    mScreenHeight = 240;

    mSceneToInstall = NULL;

    mHasGraphics = false;
}

void SceneManager::RequestNewScene(Scene* newScene) {
    LOGD("SceneManager: requesting new scene %p", newScene);
    mSceneToInstall = newScene;
}

void SceneManager::InstallScene(Scene* newScene) {
    LOGD("SceneManager: installing scene %p.", newScene);

    // kill graphics, if we have them.
    bool hadGraphics = mHasGraphics;
    if (mHasGraphics) {
        KillGraphics();
    }

    // If we have an existing scene, uninstall it.
    if (mCurScene) {
        mCurScene->OnUninstall();
        delete mCurScene;
        mCurScene = NULL;
    }

    // install the new scene
    mCurScene = newScene;
    if (mCurScene) {
        mCurScene->OnInstall();
    }

    // if we had graphics before, start them again.
    if (hadGraphics) {
        StartGraphics();
    }
}

Scene* SceneManager::GetScene() { return mCurScene; }

void SceneManager::DoFrame() {
    if (mSceneToInstall) {
        InstallScene(mSceneToInstall);
        mSceneToInstall = NULL;
    }

    if (mHasGraphics && mCurScene) {
        mCurScene->DoFrame();
    }
}

void SceneManager::KillGraphics() {
    if (mHasGraphics) {
        LOGD("SceneManager: killing graphics.");
        mHasGraphics = false;
        if (mCurScene) {
            mCurScene->OnKillGraphics();
        }
    }
}

void SceneManager::StartGraphics() {
    if (!mHasGraphics) {
        LOGD("SceneManager: starting graphics.");
        mHasGraphics = true;
        if (mCurScene) {
            LOGD("SceneManager: calling mCurScene->OnStartGraphics.");
            mCurScene->OnStartGraphics();
        }
    }
}

void SceneManager::SetScreenSize(int width, int height) {
    if (mScreenWidth != width || mScreenHeight != height) {
        mScreenWidth = width;
        mScreenHeight = height;

        if (mCurScene && mHasGraphics) {
            mCurScene->OnScreenResized(width, height);
        }
    }
}

SceneManager* SceneManager::GetInstance() { return &_sceneManager; }

void SceneManager::OnPointerDown(int pointerId,
                                 const struct PointerCoords* coords) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPointerDown(pointerId, coords);
    }
}

void SceneManager::OnPointerUp(int pointerId,
                               const struct PointerCoords* coords) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPointerUp(pointerId, coords);
    }
}

void SceneManager::OnPointerMove(int pointerId,
                                 const struct PointerCoords* coords) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPointerMove(pointerId, coords);
    }
}

bool SceneManager::OnBackKeyPressed() {
    if (mHasGraphics && mCurScene) {
        return mCurScene->OnBackKeyPressed();
    }
    return false;
}

void SceneManager::OnKeyDown(int ourKeycode) {
    MY_ASSERT(ourKeycode >= 0 && ourKeycode < OURKEY_COUNT);
    if (mHasGraphics && mCurScene) {
        mCurScene->OnKeyDown(ourKeycode);

        // if our "escape" key (normally corresponding to joystick button B or Y)
        // was pressed, handle it as a back key
        if (ourKeycode == OURKEY_ESCAPE) {
            mCurScene->OnBackKeyPressed();
        }
    }
}

void SceneManager::OnKeyUp(int ourKeycode) {
    MY_ASSERT(ourKeycode >= 0 && ourKeycode < OURKEY_COUNT);
    if (mHasGraphics && mCurScene) {
        mCurScene->OnKeyUp(ourKeycode);
    }
}

void SceneManager::UpdateJoy(float joyX, float joyY) {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnJoy(joyX, joyY);
    }
}

void SceneManager::OnPause() {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnPause();
    }
}

void SceneManager::OnResume() {
    if (mHasGraphics && mCurScene) {
        mCurScene->OnResume();
    }
}

void SceneManager::HandleInput(AInputEvent *event) {
    int32_t eventType = AInputEvent_getType(event);

    switch (eventType) {
        case AINPUT_EVENT_TYPE_MOTION: {
            int32_t action = AMotionEvent_getAction(event);
            int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int32_t pointerId = AMotionEvent_getPointerId(event, pointerIndex);

            // Get coordinates for the pointer
            PointerCoords coords;
            coords.x = AMotionEvent_getX(event, pointerIndex);
            coords.y = AMotionEvent_getY(event, pointerIndex);
            coords.minX = 0;
            coords.maxX = mScreenWidth - 1;
            coords.minY = 0;
            coords.maxY = mScreenHeight - 1;

            switch (action & AMOTION_EVENT_ACTION_MASK) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    OnPointerDown(pointerId, &coords);
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                case AMOTION_EVENT_ACTION_CANCEL:
                    OnPointerUp(pointerId, &coords);
                    break;
                case AMOTION_EVENT_ACTION_MOVE:
                    // Process all active pointers during a move event
                    for (size_t i = 0; i < AMotionEvent_getPointerCount(event); ++i) {
                        PointerCoords moveCoords;
                        moveCoords.x = AMotionEvent_getX(event, i);
                        moveCoords.y = AMotionEvent_getY(event, i);
                        moveCoords.minX = 0;
                        moveCoords.maxX = mScreenWidth - 1;
                        moveCoords.minY = 0;
                        moveCoords.maxY = mScreenHeight - 1;
                        OnPointerMove(AMotionEvent_getPointerId(event, i), &moveCoords);
                    }
                    break;
            }
            return; // Event handled
        }
        case AINPUT_EVENT_TYPE_KEY: {
            // Note: Key events are usually better handled via the NDK's keycode mapping,
            // which you seem to be doing in OnKeyDown/OnKeyUp already.
            // If you still want to handle raw key events, you can do it here.
            // For now, it's better to let the existing OnKeyDown/OnKeyUp logic handle it.
            // int32_t keyCode = AKeyEvent_getKeyCode(event);
            // int32_t keyAction = AKeyEvent_getAction(event);
            // ...
            break;
        }
    }
}
