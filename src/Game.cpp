#include "Game.h"
#include <iostream>

Game::Game() 
    : renderer(rm, uiManager), player(), scene(), fadeManager() {
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
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    if (scene.GetCurrentScene() == SceneType::Game) {
                        BeginFadeTo(SceneType::Menu);
                    } else {
                        window.close();
                    }
                }
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button != sf::Mouse::Button::Left || fadeManager.IsFading()) {
                    continue;
                }

                const sf::Vector2f mousePos{
                    static_cast<float>(mouse->position.x),
                    static_cast<float>(mouse->position.y)
                };

                if (scene.GetCurrentScene() == SceneType::Menu) {
                    if (renderer.GetStartButton().bounds.contains(mousePos)) {
                        BeginFadeTo(SceneType::Game);
                        gameState = GameState::GAME;
                    } else if (renderer.GetSettingsButton().bounds.contains(mousePos)) {
                        BeginFadeTo(SceneType::Settings);
                    } else if (renderer.GetExitButton().bounds.contains(mousePos)) {
                        window.close();
                    }
                } else if (scene.GetCurrentScene() == SceneType::Settings) {
                    BeginFadeTo(SceneType::Menu);
                }
            }
        }

        // Update logic
        Logic(dt);

        // Draw
        Draw();
    }
}

void Game::Init() {
    // Initialize renderer
    renderer.Init();

    // Setup player initial state (player stores texture keys, renderer draws)
    player.SetFeet({PlayerStartX, PlayerGroundY});
    player.SetHeight(PlayerHeight);
    player.SetSpeed(PlayerSpeed);
    //player.SetFrame(PlayerFrame::Stand);
    player.SetTextureIndex(0);

    // Initialize UI
    uiManager.Init(rm);
    uiManager.SetHP(100, 100);

    // Set initial scene
    scene.SetCurrentScene(SceneType::Menu);
    scene.SetCurrentCarriage(0);
}

void Game::Logic(float dt) {
    // Update fade effect
    fadeManager.Update(dt);

    if (fadeManager.GetState() == FadeState::None) {
        // Complete transition after fade-in finishes
        if (scene.GetPendingScene() != scene.GetCurrentScene() ||
            scene.GetPendingCarriage() != scene.GetCurrentCarriage()) {
            CompleteFadeTransition();
        }

        // Normal game logic
        if (scene.GetCurrentScene() == SceneType::Game) {
            UpdatePlayer(dt);
        }
    }
}

void Game::UpdatePlayer(float dt) {
    bool playerMoving = false;
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

    if (movementDirection != 0) {
        playerMoving = true;
        player.SetFacing(movementDirection);
        player.Move(movementDirection, dt);

        sf::Vector2f newFeet = player.GetFeet();

        // Check carriage transitions
        if (newFeet.x <= Scene::GetLeftExitX()) {
            if (scene.CanGoLeft()) {
                BeginCarriageFade(scene.GetCurrentCarriage() - 1, 
                                 Scene::GetRightEntryX(), -1);
            }
            newFeet.x = Scene::GetLeftExitX();
        } else if (newFeet.x >= Scene::GetRightExitX()) {
            if (scene.CanGoRight()) {
                BeginCarriageFade(scene.GetCurrentCarriage() + 1, 
                                 Scene::GetLeftEntryX(), 1);
            }
            newFeet.x = Scene::GetRightExitX();
        }

        player.SetFeet(newFeet);
    }

    // Update player animation state
    player.SetMoving(playerMoving);
    if (!playerMoving) {
        player.ResetToStand();
    }

    player.UpdateSpritePosition();
}

void Game::BeginFadeTo(SceneType nextScene) {
    if (fadeManager.IsFading()) {
        return;
    }

    scene.SetPendingScene(nextScene);
    scene.SetPendingCarriage(scene.GetCurrentCarriage());
    pendingPlayerFeet = player.GetFeet();
    pendingPlayerFacing = player.GetFacing();
    
    fadeManager.StartFadeOut();
}

void Game::BeginCarriageFade(int nextCarriage, float nextX, int nextFacing) {
    if (fadeManager.IsFading()) {
        return;
    }

    scene.SetPendingScene(SceneType::Game);
    scene.SetPendingCarriage(nextCarriage);
    pendingPlayerFeet = {nextX, PlayerGroundY};
    pendingPlayerFacing = nextFacing;
    
    fadeManager.StartFadeOut();
}

void Game::CompleteFadeTransition() {
    scene.SetCurrentScene(scene.GetPendingScene());
    scene.SetCurrentCarriage(scene.GetPendingCarriage());
    player.SetFeet(pendingPlayerFeet);
    player.SetFacing(pendingPlayerFacing);
}

void Game::Draw() {
    window.clear();

    SceneType currentScene = scene.GetCurrentScene();
    
    if (currentScene == SceneType::Menu) {
        renderer.DrawMenu(window);
    } else if (currentScene == SceneType::Game) {
        renderer.DrawGame(window, player);
    } else if (currentScene == SceneType::Settings) {
        renderer.DrawSettings(window);
    }

    if (fadeManager.GetAlpha() > 0.f) {
        renderer.DrawFadeOverlay(window, static_cast<std::uint8_t>(fadeManager.GetAlpha()));
    }

    window.display();
}