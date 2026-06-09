#pragma once

#include "Constants.h"
#include "TimeSystem.h"
#include "Renderer.h"
#include "Input.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "Player.h"
#include "UIManager.h"
#include "FadeManager.h"
#include <windows.h>
#include <string>
#include <unordered_map>
#include <vector>

using namespace sf;

class Game {
public:
    Game();
    ~Game();

    void Run();

private:
    unsigned int windowWidth = 1920;
    unsigned int windowHeight = 1080;
    RenderWindow window;

    GameState gameState = GameState::MENU;

    // Game systems
    ResourceManager rm;
    Renderer renderer;
    Input input;
    TimeSystem timeSystem;
    UIManager uiManager;
    FadeManager fadeManager;

    // Game objects
    Player player;
    Scene scene;

    // Game constants
    static constexpr float PlayerGroundY = 1028.f;
    static constexpr float PlayerHeight = 454.f;
    static constexpr float PlayerStartX = 350.f;
    static constexpr float PlayerSpeed = 430.f;

    // Game state variables
    sf::Vector2f pendingPlayerFeet = {PlayerStartX, PlayerGroundY};
    int pendingPlayerFacing = 1;

    // Main loop methods
    void Init();
    void Logic(float dt);
    void HandleInput();
    void UpdatePlayer(float dt);
    void UpdateScene();
    void Draw();

    // Fade transition control
    void BeginFadeTo(SceneType nextScene);
    void BeginCarriageFade(int nextCarriage, float nextX, int nextFacing);
    void CompleteFadeTransition();

    // Saved game interface
    GameData GetGameData() const {
        GameData data;
        return data;
    }
    void SetGameData(const GameData& data) {
    }
};
