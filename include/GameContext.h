#pragma once

class UIManager;
class SceneManager;
class AnimationManager;
class ResourceManager;
class AudioManager;

class GameContext {
public:

    UIManager* ui = nullptr;
    SceneManager* scene = nullptr;
    AnimationManager* anim = nullptr;
    ResourceManager* rm = nullptr;
    AudioManager* audio = nullptr;
};