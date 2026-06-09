#pragma once

#include "Constants.h"
#include "TimeSystem.h"
#include "Renderer.h"
#include "Input.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Player.h"
#include "UIManager.h"
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
    UIManager uiMgr;
    SceneManager sceneMgr;

    // Game objects
    Player player;
    //std::vector<::GameEvent> gameEvents;

    // Main loop methods
    void Init();
    void Logic(float dt);
    void HandleInput(float dt);
    void ProcessEvents();
    void UpdatePlayer(float dt);
    void Draw();

    // Saved game interface
    GameData GetGameData() const {
        GameData data;
        return data;
    }
    void SetGameData(const GameData& data) {
    }
};
