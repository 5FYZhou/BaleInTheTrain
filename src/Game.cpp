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
    uiMgr.Init(rm);
    audioMgr.Initialize(rm);
    audioMgr.SetSfxVolume(DEFAULT_SFX_VOLUME);
    audioMgr.SetMusicVolume(DEFAULT_MUSIC_VOLUME);
    dialogMgr.Initialize(&audioMgr);
    player.Init(rm.getTextureCount(TextureType::Player));
    renderer.Init();
    sceneMgr.SetCurScene(SceneType::Menu);
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
    player.SetHP(100, 100);

    keyCnt = 0;

    // 暂时指定卡片
    cardsOnPlayer = { PileType::Strike, PileType::Defend, PileType::Defend, PileType::Strike, PileType::Strike};
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
                } 
                /*
                else if (sceneMgr.GetCurSceneType() == SceneType::Game) {
                    sceneMgr.LoadScene(SceneType::Menu);
                } 
                else if(sceneMgr.GetCurSceneType() == SceneType::Battle){
                    sceneMgr.LoadGameBeforeBattle([this]{
                        uiMgr.CloseCardsInHandPopup();
                        player.SetFacing(playerFaceBeforeBattle);
                        player.SetFeet({playerXBeforeBattle, PlayerGroundY});
                        player.ResetToStand();
                    });
                }*/
                else if(sceneMgr.GetCurSceneType() == SceneType::Menu) {
                    window.close();
                }
            } 
            else if (sceneMgr.GetCurSceneType() == SceneType::Game &&
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

            const sf::Vector2i mousePosi = sf::Mouse::getPosition(window);
            const sf::Vector2f mousePos{
                static_cast<float>(mousePosi.x),
                static_cast<float>(mousePosi.y)
            };

            // 如果有UI面板是打开的或处理了该鼠标点击 跳过场景处理鼠标点击
            if(uiMgr.HandleMousePressed(mousePos)) 
                return;

            if (sceneMgr.GetCurSceneType() == SceneType::Game && dialogMgr.IsActive()) {
                dialogMgr.AdvanceDialog();
                continue;
            }

            sceneMgr.GetScene().ProcessInput(mousePos);
        }
    }

    playerMoveDir = 0;
    // Check keyboard input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
        playerMoveDir -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
        playerMoveDir += 1;
    }

    
}

void Game::Logic(float dt) {
    sceneMgr.Update(dt);
    dialogMgr.Update(dt);
    audioMgr.Update();
    if (!sceneMgr.IsFading() && !dialogMgr.IsActive() && !uiMgr.BlockInput()) {
        if (sceneMgr.GetCurSceneType() == SceneType::Game) {
            // 没撞墙
            float nextX = player.GetNextX(playerMoveDir, dt);
            bool canMove = sceneMgr.CheckChangeGameScene(nextX, keyCnt);
            if(canMove){
                player.Move(playerMoveDir, dt);
            }
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
        #pragma region 切场景,UI,音乐
        // 切换到游戏场景
        case EventType::StartGame:
            sceneMgr.LoadScene(SceneType::Game);
            dialogMgr.StartDialog();
            audioMgr.PlaySound(SoundEffect::MenuButton);
            break;
        // 退出游戏
        case EventType::ExitGame:
            window.close();
            break;
        // 走到车厢开头/末尾时钥匙数量不足
        case EventType::KeysInsufficient:
            std::cout<<"Event::KeysInsufficient"<<std::endl;
            break;
        // 钥匙足够打开门
        case EventType::Win:
            std::cout<<"Event::Win"<<std::endl;
            sceneMgr.LoadScene(SceneType::Win);
            break;
        case EventType::ReturnMenu:
            std::cout<<"Event::ReturnMenu"<<std::endl;
            sceneMgr.LoadScene(SceneType::Menu);
            Init();
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
        // 打开弃牌池
        case EventType::OpenDiscardPile:
            std::cout<<"Event::openDiscard"<<std::endl;
            uiMgr.SetDiscardCard(cardsOnPlayer);
            uiMgr.OpenDiscardPopup();
            break;
        // 打开发牌池
        case EventType::OpenDealCardPanel:
            std::cout<<"Event::openDealcard"<<std::endl;
            uiMgr.SetDealCardCard(cardsOnPlayer);
            uiMgr.OpenDealCardPopup();
            break;

        // 修改音效音量
        case EventType::SfxVolumeChange:
            audioMgr.SetSfxVolume(event.val);
            break;
        // 修改音乐音量
        case EventType::MusicVolumeChange:
            audioMgr.SetMusicVolume(event.val);
            break;
        #pragma endregion
        
        // 进入新游戏/战斗场景时 重置玩家位置
        case EventType::ResetPlayerPos:
            player.SetFeet({event.val, PlayerGroundY});
            if(event.val == PlayerStartX){
                player.SetFacing(1);
            }
            player.ResetToStand();
            std::cout<< "Event: Reset player position to: " << player.GetFeet().x << std::endl;
            break; 
        // 场景中的交互物品点击
        case EventType::ItemClicked:
        {
            ItemType itemtype = static_cast<ItemType>(event.val);
            switch (itemtype)
            {
            case ItemType::Enemy:
                if(uiMgr.HasSelectedCard()){
                    // 出牌
                    auto [type, idx] = uiMgr.GetSelectedCard();
                    std::cout<<"event: click Enemy & play card:"<<static_cast<int>(type)<<" idx:"<<idx<<std::endl;
                    // 如果出牌 删除该牌
                    cardsOnPlayer.erase(cardsOnPlayer.begin() + idx);
                    // 重新绘制
                    uiMgr.SetCardsInHandCard(cardsOnPlayer);
                    uiMgr.SetHasSelected(false);
                }
                break;
            case ItemType::Player:
                if(uiMgr.HasSelectedCard()){
                    // 出牌
                    auto [type, idx] = uiMgr.GetSelectedCard();
                    std::cout<<"event: click Player & play card:"<<static_cast<int>(type)<<" idx:"<<idx<<std::endl;
                    // 如果出牌 删除该牌
                    cardsOnPlayer.erase(cardsOnPlayer.begin() + idx);
                    // 重新绘制
                    uiMgr.SetCardsInHandCard(cardsOnPlayer);
                    uiMgr.SetHasSelected(false);
                }
                break;
            case ItemType::Key:
                std::cout<<"Event: get one key"<<std::endl;
                keyCnt++;
                break;
                
            case ItemType::Strike: //打击0
            case ItemType::Defend: //防御1
            case ItemType::Rage: //暴走2
            case ItemType::Shrug_off: //耸肩无视3
            case ItemType::Heavy_strike: //痛击4
            case ItemType::Anger: //愤怒5
            case ItemType::Continuous_punches: //连续拳6
            case ItemType::Observe_weaknesses: //观察弱点7
            case ItemType::Activate_muscles: //活动肌肉8
            case ItemType::Revitalize_spirit: //重振精神9
            case ItemType::Metallization: //金属化10
            case ItemType::Unstoppable: //势不可挡11
            case ItemType::Rampart: //壁垒12
            case ItemType::Sacrifice: //祭品13
                std::cout<< "Event : hitcard:"<<event.val<<std::endl; 
                // 给玩家加卡牌
                cardsOnPlayer.push_back(itemPileMap.at(itemtype));
                break;
            default:
                break;
            }
        }
            break;
        // 开始战斗
        case EventType::BeginBattle:
            playerFaceBeforeBattle = player.GetFacing();
            playerXBeforeBattle = player.GetPos().x;
            sceneMgr.LoadScene(SceneType::Battle, [this]{
                uiMgr.SetCardsInHandCard(cardsOnPlayer);
                uiMgr.OpenCardsInHandPopup();
                player.SetFacing(1);
            });
            std::cout<<"Event:beginBattle"<<std::endl;
            break;
        // 结束回合
        case EventType::EndTurn:
            std::cout<<"Event: end turn"<<std::endl;
            btLogic.turnsOver();
            break;
        // 结束对局
        case EventType::EndBattle:
            std::cout<<"Event: EndBattle"<<std::endl;
            // 胜利
            if(event.val == 0){
                sceneMgr.LoadGameBeforeBattle([this]{
                    uiMgr.CloseCardsInHandPopup();
                    player.SetFacing(playerFaceBeforeBattle);
                    player.SetFeet({playerXBeforeBattle, PlayerGroundY});
                    player.ResetToStand();
                });
            }
            // 失败
            else if(event.val == 1){

            }
            break;
        default:
            break;
        }
}

void Game::Draw() {
    window.clear();

    SceneType curSceneType = sceneMgr.GetCurSceneType();
    
    // 绘制场景
    renderer.DrawScene(window, sceneMgr.GetScene());
    
    if(curSceneType == SceneType::Game || curSceneType == SceneType::Battle){
        renderer.DrawPlayer(window, player);
    }

    if(curSceneType == SceneType::Game) {
        renderer.DrawDialog(window, dialogMgr);
        renderer.DrawCardRewards(window, cardsOnPlayer, dialogMgr.GetRewardAlpha());
        if (dialogMgr.IsMovementHintVisible()) {
            renderer.DrawMovementHint(window);
        }
    }

    // UI面板
    uiMgr.DrawPanels(window, mousePosMove);

    // 切场景遮罩
    if (sceneMgr.GetFadeAlpha() > 0.f) {
        renderer.DrawFadeOverlay(window, static_cast<std::uint8_t>(sceneMgr.GetFadeAlpha()));
    }

    window.display();
}
