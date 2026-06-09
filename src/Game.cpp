#include "Game.h"
#include <iostream>

Game::Game() 
    : renderer(rm, uiMgr), player(), sceneMgr() {
    windowWidth = 1920;
    windowHeight = 1080;

    uint8_t WindowStyle = sf::Style::Close | sf::Style::Titlebar;
    window.create(sf::VideoMode({
        static_cast<unsigned int>(windowWidth),
        static_cast<unsigned int>(windowHeight)
    }), "Bale In The Train", WindowStyle);

    window.setFramerateLimit(60);
    gameState = GameState::MENU;
}

Game::~Game() {
    // Cleanup will be done by player and renderer destructors
}

void Game::Run() {
    Init();
    
    while (window.isOpen()) {
        float dt = timeSystem.GetDeltaTime();

        // Handle events
        HandleInput(dt);

        // Update logic
        Logic(dt);

        // Draw
        Draw();
    }
}

void Game::Init() {
    renderer.Init();

    player.Init(rm.getTextureCount(TextureType::Player));

    uiMgr.Init(rm);
    uiMgr.SetHP(100, 100);

    sceneMgr.SetCurScene(SceneType::Menu);
}

void Game::HandleInput(float dt){
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->scancode == sf::Keyboard::Scancode::Escape) {
                if (sceneMgr.GetCurrentSceneType() == SceneType::Game) {
                    sceneMgr.LoadScene(SceneType::Menu);
                } else {
                    window.close();
                }
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button != sf::Mouse::Button::Left || sceneMgr.IsFading()) {
                continue;
            }

            const MouseState mouseState = input.GetMouseState(window);
            const sf::Vector2f mousePos{
                static_cast<float>(mouseState.position.x),
                static_cast<float>(mouseState.position.y)
            };

            sceneMgr.GetScene().ProcessInput(mousePos);
        }
    }

    int movementDirection = 0;
        // Check keyboard input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
            movementDirection -= 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
            movementDirection += 1;
        }
        player.Move(movementDirection, dt);
}

void Game::Logic(float dt) {
    // Update fade effect through SceneManager
    sceneMgr.Update(dt);

    if (!sceneMgr.IsFading()) {
        if (sceneMgr.GetCurrentSceneType() == SceneType::Game) {
            //UpdatePlayer(dt);
            sceneMgr.CheckChangeGameScene(player.GetFeet().x);
        }
    }
    
    ProcessEvents();
}

void Game::UpdatePlayer(float dt) {
}

void Game::ProcessEvents() {
    std::vector<GameEvent>& eventsInScene = sceneMgr.GetEvents();
    for (const auto& event : eventsInScene) {
        switch (event.type) {
            case EventType::StartGame:
                sceneMgr.LoadScene(SceneType::Game);
                gameState = GameState::GAME;
                break;
            case EventType::OpenSettings:
                //sceneManager.LoadScene(SceneType::Settings);
                break;
            case EventType::ExitGame:
                window.close();
                break;
            case EventType::ResetPlayerPos:
                player.SetFeet({event.val, PlayerGroundY});
                if(event.val == PlayerStartX){
                    player.SetFacing(1);
                }
                player.ResetToStand();
                std::cout<< "Reset player position to: " << player.GetFeet().x << std::endl;
                break;
            case EventType::ItemClicked:
                // TODO: 处理游戏场景中的交互物品点击事件
                std::cout<< "hitItem:"<<event.val<<std::endl;
                break;
            default:
                break;
        }
    }
    eventsInScene.clear();
}

void Game::Draw() {
    window.clear();

    SceneType currentScene = sceneMgr.GetCurrentSceneType();
    
    if (currentScene == SceneType::Menu) {
        renderer.DrawMenu(window);
    } else if (currentScene == SceneType::Game) {
        renderer.DrawGame(window, player);
    } else if (currentScene == SceneType::Settings) {
        renderer.DrawSettings(window);
    }

    Scene& scene = sceneMgr.GetScene();
    for(const auto& item : scene.GetInteractables()) {
        renderer.DrawItem(window, item.position, item.texture);
    }

    if (sceneMgr.GetFadeAlpha() > 0.f) {
        renderer.DrawFadeOverlay(window, static_cast<std::uint8_t>(sceneMgr.GetFadeAlpha()));
    }

    window.display();
}