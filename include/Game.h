#pragma once

#include "Tools/AudioManager.h"
#include "Constants.h"
#include "UI/TextHintManager.h"
#include "Input.h"
#include "Player.h"
#include "Renderer.h"
#include "Tools/ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Tools/TimeSystem.h"
#include "BattleLogic.h"
#include "UI/UIManager.h"
#include "GameContext.h"
#include <SFML/Graphics.hpp>
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
    
    GameContext ctx;
    ResourceManager rm;
    Renderer renderer;
    TimeSystem timeSystem;
    UIManager uiMgr;
    SceneManager sceneMgr;
    AudioManager audioMgr;
    TextHintManager textHintMgr;
    BattleLogic btLogic;
    
    Player player;
    int playerMoveDir;
    int keyCnt;
    float playerXBeforeBattle;
    int playerFaceBeforeBattle;

    void Init();
    void Logic(float dt);
    void HandleInput(float dt);
    void ProcessEvents();
    void HandleEvents(const GameEvent& event);
    void Draw();

    GameData GetGameData() const {
        GameData data;
        return data;
    }

    void SetGameData(const GameData& data) {
        (void)data;
    }
};
