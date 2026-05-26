#pragma once

#include "Constants.h"
#include "TimeSystem.h"
#include "Renderer.h"
#include "Input.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "Player.h"
#include <windows.h>
#include <string>
#include <unordered_map>
#include <vector>
using namespace sf;

class Game {
public:
    Game();
    void Run();

private:
    int windowWidth, windowHeight;
    RenderWindow window;

    GameState gameState;

    ResourceManager rm;
    Renderer renderer;
    Input input;
    TimeSystem timeSystem;

    Player player;
    Scene scene;

    void Init();
    void Logic();
    void HandleInput();
    void Draw();

    // ...保留存档相关接口...
    GameData GetGameData() const {
        GameData data;
        return data;
    }
    void SetGameData(const GameData& data) {
    }
};
