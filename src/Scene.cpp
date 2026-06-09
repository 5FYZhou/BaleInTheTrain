#include "Scene.h"

Scene::Scene() {
    // Initialize carriage system
    currentScene = SceneType::Menu;
    currentCarriage = 0;
}