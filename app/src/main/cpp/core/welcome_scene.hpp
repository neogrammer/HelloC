//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_WELCOME_SCENE_HPP
#define HELLOC_WELCOME_SCENE_HPP


#include "engine.hpp"
#include "our_shader.hpp"
#include "tex_quad.hpp"
#include "scenes/ui_scene.hpp"
#include "util.hpp"

/* The "welcome scene" (main menu) */
class WelcomeScene : public UiScene {
protected:
    // IDs for our buttons:
    int mPlayButtonId;
    int mStoryButtonId;
    int mAboutButtonId;

    virtual void RenderBackground();
    virtual void OnButtonClicked(int id);

    void UpdateWidgetStates();

public:
    WelcomeScene();
    ~WelcomeScene();

    virtual void OnCreateWidgets();
    virtual void OnStartGraphics();
    virtual void OnKillGraphics();

    virtual void DoFrame();
};

#endif //HELLOC_WELCOME_SCENE_HPP
