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

    audioMgr.Initialize(rm);
    audioMgr.SetSfxVolume(DEFAULT_SFX_VOLUME);
    audioMgr.SetMusicVolume(DEFAULT_MUSIC_VOLUME);
    dialogMgr.Initialize(&audioMgr);

    sceneMgr.SetCurScene(SceneType::Menu);

    // 暂时指定卡片
    cardsOnPlayer = { CardType::Strike6, CardType::Defend5, CardType::Defend5, CardType::Strike6, CardType::Strike6};
}

void Game::HandleInput(float dt){
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->scancode == sf::Keyboard::Scancode::Escape) {
                if (uiMgr.IsSettingsPopupOpen()) {
                    uiMgr.CloseSettingsPopup();
                    audioMgr.PlaySound(SoundEffect::Back);
                } else if (sceneMgr.GetCurrentSceneType() == SceneType::Game) {
                    sceneMgr.LoadScene(SceneType::Menu);
                } else {
                    window.close();
                }
            } 
            else if (sceneMgr.GetCurrentSceneType() == SceneType::Game &&
                       dialogMgr.IsActive() &&
                       (key->scancode == sf::Keyboard::Scancode::Space ||
                        key->scancode == sf::Keyboard::Scancode::Enter)) {
                dialogMgr.AdvanceDialog();
            }
        }

        if (const auto* mouseMove = event->getIf<sf::Event::MouseMoved>()) {
            uiMgr.HandleMouseMoved({
                static_cast<float>(mouseMove->position.x),
                static_cast<float>(mouseMove->position.y)
            });
            mousePosMove = mouseMove->position;
        }

        if (const auto* mouseRelease = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseRelease->button == sf::Mouse::Button::Left) {
                uiMgr.HandleMouseReleased();
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

            // 如果有UI面板是打开的或处理了该鼠标点击 跳过场景处理鼠标点击
            if(uiMgr.HandleMousePressed(mousePos)) 
                return;

            if (sceneMgr.GetCurrentSceneType() == SceneType::Game && dialogMgr.IsActive()) {
                dialogMgr.AdvanceDialog();
                continue;
            }

            sceneMgr.GetScene().ProcessInput(mousePos);
        }
    }

    int movementDirection = 0;
    if (sceneMgr.GetCurrentSceneType() != SceneType::Game ||
        dialogMgr.IsActive() ||
        uiMgr.IsSettingsPopupOpen() ||
        sceneMgr.IsFading()) {
        player.Move(0, dt);
        return;
    }

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
    sceneMgr.Update(dt);
    dialogMgr.Update(dt);
    audioMgr.Update();

    if (!sceneMgr.IsFading()) {
        if (sceneMgr.GetCurrentSceneType() == SceneType::Game) {
            sceneMgr.CheckChangeGameScene(player.GetFeet().x);
        }
    }
    
    ProcessEvents();
}

void Game::ProcessEvents() {
    std::vector<GameEvent>& eventsInScene = sceneMgr.GetEvents();
    for (const auto& event : eventsInScene) {
        HandleEvents(event);
    }
    eventsInScene.clear();

    std::vector<GameEvent>& eventsInUI = uiMgr.GetEvents();
    for (const auto& event : eventsInUI) {
        HandleEvents(event);
    }
    eventsInUI.clear();
}

void Game::HandleEvents(const GameEvent& event){
    switch (event.type) {
        // 切换到游戏场景
        case EventType::StartGame:
            sceneMgr.LoadScene(SceneType::Game);
            gameState = GameState::GAME;
            dialogMgr.StartDialog();
            audioMgr.PlaySound(SoundEffect::MenuButton);
            break;
        // 退出游戏
        case EventType::ExitGame:
            window.close();
            break;

        // 打开设置面板
        case EventType::OpenSettings:
            std::cout<<"Event::opensettings"<<std::endl;
            uiMgr.OpenSettingsPopup();
            audioMgr.PlaySound(SoundEffect::MenuButton);
            break;
        // 打开背包面板
        case EventType::OpenBackpackIcon:
            std::cout<<"Event::openbackpack"<<std::endl;
            uiMgr.SetBackPackCard(cardsOnPlayer);
            uiMgr.OpenBackpackPopup();
            break;

        // 修改音效音量
        case EventType::SfxVolumeChange:
            audioMgr.SetSfxVolume(event.val);
            break;
        // 修改音乐音量
        case EventType::MusicVolumeChange:
            audioMgr.SetMusicVolume(event.val);
            break;

        // 进入新游戏场景时 重置玩家位置
        case EventType::ResetPlayerPos:
            player.SetFeet({event.val, PlayerGroundY});
            if(event.val == PlayerStartX){
                player.SetFacing(1);
            }
            player.ResetToStand();
            std::cout<< "Reset player position to: " << player.GetFeet().x << std::endl;
            break;
        // 场景中的交互物品点击
        case EventType::ItemClicked:
            // TODO: 处理游戏场景中的交互物品点击事件
            std::cout<< "hitItem:"<<event.val<<std::endl;
            break;
        default:
            break;
        }
}

void Game::Draw() {
    window.clear();

    SceneType currentScene = sceneMgr.GetCurrentSceneType();
    
    if (currentScene == SceneType::Menu) {
        renderer.DrawMenu(window);
    } else if (currentScene == SceneType::Game) {
        renderer.DrawGame(window, player);
        uiMgr.DrawDialog(window, dialogMgr);
        uiMgr.DrawCardRewards(window, cardsOnPlayer, dialogMgr.GetRewardAlpha());
        if (dialogMgr.IsMovementHintVisible()) {
            uiMgr.DrawMovementHint(window);
        }
    }

    // 场景中的交互物品
    Scene& scene = sceneMgr.GetScene();
    for(const auto& item : scene.GetInteractables()) {
        renderer.DrawItem(window, item.position, item.texture, item.scale);
    }

    // 切场景遮罩
    if (sceneMgr.GetFadeAlpha() > 0.f) {
        renderer.DrawFadeOverlay(window, static_cast<std::uint8_t>(sceneMgr.GetFadeAlpha()));
    }

    uiMgr.DrawPanels(window, mousePosMove);

    window.display();
}
