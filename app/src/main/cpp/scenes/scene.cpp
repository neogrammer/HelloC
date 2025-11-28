#include "scene.hpp"

// Default implementations of virtual methods.
Scene::Scene() {}
Scene::~Scene() = default;
void Scene::OnInstall() {}
void Scene::DoFrame() {}
void Scene::OnUninstall() {}
void Scene::OnStartGraphics() {}
void Scene::OnKillGraphics() {}
void Scene::OnPointerDown(int, const struct PointerCoords*) {}
void Scene::OnPointerUp(int, const struct PointerCoords*) {}
void Scene::OnPointerMove(int, const struct PointerCoords*) {}
bool Scene::OnBackKeyPressed() { return false; }
void Scene::OnKeyDown(int) {}
void Scene::OnKeyUp(int) {}
void Scene::OnJoy(float, float) {}
void Scene::OnScreenResized(int, int) {}
void Scene::OnPause() {}
void Scene::OnResume() {}
