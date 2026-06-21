#include "Game.h"
#include <iostream>

Game::Game()
    : renderer(rm, uiMgr), player(), sceneMgr()
{
    windowWidth = 1920;
    windowHeight = 1080;

    uint8_t WindowStyle = sf::Style::Close | sf::Style::Titlebar;
    window.create(sf::VideoMode({static_cast<unsigned int>(windowWidth),
                                 static_cast<unsigned int>(windowHeight)}),
                  "Bale In The Train", WindowStyle);

    window.setFramerateLimit(60);
    uiMgr.Init(rm);
    audioMgr.Initialize(rm);
    audioMgr.SetSfxVolume(DEFAULT_SFX_VOLUME);
    audioMgr.SetMusicVolume(DEFAULT_MUSIC_VOLUME);
    textHintMgr.Initialize(&audioMgr);
    renderer.Init();
    sceneMgr.SetCurScene(SceneType::Menu);

    // ctx.ui = &uiMgr;
    // ctx.scene = &sceneMgr;
    // ctx.anim = uiMgr.GetAnimationManager(); // 或 uiMgr.anim
    // ctx.rm = &rm;
    // ctx.audio = &audioMgr;

    btLogic.textPrompt = &uiMgr.textPrompt;

    player.Init(rm.getTextureCount(TextureType::Player));
}

Game::~Game()
{
    // Cleanup will be done by player and renderer destructors
}

void Game::Run()
{
    Init();

    while (window.isOpen())
    {
        float dt = timeSystem.GetDeltaTime();

        // Handle events
        HandleInput(dt);

        // Update logic
        Logic(dt);

        // Draw
        Draw();
    }
}

void Game::Init()
{
    player.ResetExceptCard();
    keyCnt = 3;
    player.InitCards();
    playerDeadCnt = 0;
    sceneMgr.Init();
    uiMgr.rewardAni.hasInit = false;
    textHintMgr.ResetState();
}

void Game::HandleInput(float dt)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        // 切场景时禁用输入
        if(sceneMgr.IsFading()) return;

        if (const auto *key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->scancode == sf::Keyboard::Scancode::Escape)
            {
                if (uiMgr.IsOpen(PanelType::Setting))
                {
                    uiMgr.Close(PanelType::Setting);
                    audioMgr.PlaySound(SoundEffect::Back);
                }
                if (uiMgr.IsOpen(PanelType::Backpack))
                {
                    uiMgr.Close(PanelType::Backpack);
                    audioMgr.PlaySound(SoundEffect::Back);
                }
                if (uiMgr.IsOpen(PanelType::DealCard))
                {
                    uiMgr.Close(PanelType::DealCard);
                    audioMgr.PlaySound(SoundEffect::Back);
                }
                if (uiMgr.IsOpen(PanelType::Discard))
                {
                    uiMgr.Close(PanelType::Discard);
                    audioMgr.PlaySound(SoundEffect::Back);
                }
                if (sceneMgr.GetCurSceneType() == SceneType::Menu)
                {
                    window.close();
                }
            }
            else if (sceneMgr.GetCurSceneType() == SceneType::Game &&
                     textHintMgr.IsActive() &&
                     (key->scancode == sf::Keyboard::Scancode::Space ||
                      key->scancode == sf::Keyboard::Scancode::Enter))
            {
                textHintMgr.AdvanceDialog();
            }
        }

        if (const auto *mouseMove = event->getIf<sf::Event::MouseMoved>())
        {
            uiMgr.HandleMouseMoved({static_cast<float>(mouseMove->position.x),
                                    static_cast<float>(mouseMove->position.y)});
        }

        if (const auto *mouseRelease = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseRelease->button == sf::Mouse::Button::Left)
            {
                uiMgr.HandleMouseReleased();
            }
        }

        if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button != sf::Mouse::Button::Left || sceneMgr.IsFading())
            {
                continue;
            }

            const sf::Vector2i mousePosi = sf::Mouse::getPosition(window);
            const sf::Vector2f mousePos{
                static_cast<float>(mousePosi.x),
                static_cast<float>(mousePosi.y)};

            // 如果有UI面板是打开的或处理了该鼠标点击 跳过场景处理鼠标点击
            if (uiMgr.HandleMousePressed(mousePos))
                return;

            if (sceneMgr.GetCurSceneType() == SceneType::Game && textHintMgr.IsActive())
            {
                textHintMgr.AdvanceDialog();
                continue;
            }

            sceneMgr.GetScene().ProcessInput(mousePos);
        }
    }

    playerMoveDir = 0;
    // Check keyboard input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
    {
        playerMoveDir -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
    {
        playerMoveDir += 1;
    }
}

void Game::Logic(float dt)
{
    sceneMgr.Update(dt);
    textHintMgr.Update(dt);
    audioMgr.Update();
    uiMgr.Update(dt);
    if (!sceneMgr.IsFading() && !textHintMgr.IsActive() && !uiMgr.BlockInput())
    {
        if (sceneMgr.GetCurSceneType() == SceneType::Game)
        {
            // 没撞墙
            float nextX = player.GetNextX(playerMoveDir, dt);
            bool canMove = sceneMgr.CheckChangeGameScene(nextX, keyCnt);
            if (canMove)
            {
                player.Move(playerMoveDir, dt);
            }
        }
    }

    // 判断是否显示对话奖励卡牌
    if (textHintMgr.ShouldShowRewardCards())
    {
        uiMgr.rewardAni.Set(player.GetPileCards());
    }
    if (textHintMgr.ShouldStartRewardAnimation())
    {
        uiMgr.rewardAni.Start();
    }
    ProcessEvents();
}

void Game::ProcessEvents()
{
    std::vector<GameEvent> &eventsInScene = sceneMgr.GetEvents();
    for (const auto &event : eventsInScene)
    {
        HandleEvents(event);
    }
    eventsInScene.clear();

    std::vector<GameEvent> &eventsInUI = uiMgr.GetEvents();
    for (const auto &event : eventsInUI)
    {
        HandleEvents(event);
    }
    eventsInUI.clear();

    std::vector<GameEvent> &eventsInBl = btLogic.events;
    for (const auto &event : eventsInBl)
    {
        HandleEvents(event);
    }
    eventsInBl.clear();
}

void Game::HandleEvents(const GameEvent &event)
{
    switch (event.type)
    {
#pragma region 切场景,UI,音乐
    // 切换到游戏场景
    case EventType::StartGame:
        uiMgr.CloseAll();
        sceneMgr.LoadScene(SceneType::Game);
        textHintMgr.StartDialog();
        audioMgr.PlaySound(SoundEffect::MenuButton);
        break;
    // 退出游戏
    case EventType::ExitGame:
        window.close();
        break;
    // 走到车厢开头/末尾时钥匙数量不足
    case EventType::KeysInsufficient:
        std::cout << "Event::KeysInsufficient" << std::endl;
        textHintMgr.ShowDoorHint();
        break;
    // 钥匙足够打开门
    case EventType::Win:
        std::cout << "Event::Win" << std::endl;
        sceneMgr.LoadScene(SceneType::Win);
        break;
    // 胜利/死亡场景返回菜单
    case EventType::ReturnMenu:
        uiMgr.CloseAll();
        std::cout << "Event::ReturnMenu" << std::endl;
        sceneMgr.LoadScene(SceneType::Menu);
        Init();
        break;

    // 打开设置面板
    case EventType::OpenSettings:
    {
        // std::cout<<"Event::opensettings"<<std::endl;
        // uiMgr.OpenSettingsPopup();
        auto t0 = clock();

        // 打开设置
        uiMgr.Open(PanelType::Setting);

        auto t1 = clock();
        // std::cout << "OpenSettings cost: "<< (t1 - t0) << std::endl;
        audioMgr.PlaySound(SoundEffect::MenuButton);
    }
    break;
    // 打开背包面板
    case EventType::OpenBackpackIcon:
        std::cout << "Event::openbackpack" << std::endl;
        uiMgr.Get<BackpackPanel>()->SetCards(player.GetPileCards());
        uiMgr.Open(PanelType::Backpack);
        break;
    // 打开弃牌池
    case EventType::OpenDiscardPile:
        std::cout << "Event::openDiscard" << std::endl;
        uiMgr.Get<DiscardPilePanel>()->SetCards(btLogic.getdisCardPile());
        uiMgr.Open(PanelType::Discard);
        break;
    // 打开发牌池
    case EventType::OpenDealCardPanel:
        std::cout << "Event::openDealcard" << std::endl;
        uiMgr.Get<DealCardPanel>()->SetCards(btLogic.getdealCardsPile());
        uiMgr.Open(PanelType::DealCard);
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
        if (event.val == PlayerStartX)
        {
            player.SetFacing(1);
        }
        player.ResetToStand();
        std::cout << "Event: Reset player position to: " << player.GetFeet().x << std::endl;
        break;
    // 场景中的交互物品点击
    case EventType::ItemClicked:
    {
        ItemType itemtype = static_cast<ItemType>(event.val);
        switch (itemtype)
        {
        case ItemType::Enemy:
        {
            auto panel = uiMgr.Get<CardsInHandPanel>();
            if (panel->HasSelectedCard())
            {
                if (!btLogic.state.isPlayerTurn)
                    break;
                // 出牌
                auto [type, idx] = panel->GetSelectedCard();
                std::cout << "event: click Enemy & play card:" << static_cast<int>(type) << " idx:" << idx << std::endl;
                btLogic.ClickEnemy(idx);

                // 重新绘制
                panel->SetCards(btLogic.getHandCardsPile(), btLogic.state.actionPoints);
                panel->SetHasSelected(false);
            }
        }
        break;
        case ItemType::Player:
        {
            auto panel = uiMgr.Get<CardsInHandPanel>();
            if (panel->HasSelectedCard())
            {
                if (!btLogic.state.isPlayerTurn)
                    break;
                // 出牌
                auto [type, idx] = panel->GetSelectedCard();
                std::cout << "event: click Player & play card:" << static_cast<int>(type) << " idx:" << idx << std::endl;
                btLogic.ClickPlayer(idx);
                // 重新绘制
                panel->SetCards(btLogic.getHandCardsPile(), btLogic.state.actionPoints);
                panel->SetHasSelected(false);
            }
        }
        break;
        case ItemType::Key:
            std::cout << "Event: get one key" << std::endl;
            keyCnt++;
            uiMgr.PushNotification("Obtain a key", itemTexMap.at(itemtype));
            break;

        case ItemType::Strike:             // 打击0
        case ItemType::Defend:             // 防御1
        case ItemType::Rage:               // 暴走2
        case ItemType::Shrug_off:          // 耸肩无视3
        case ItemType::Heavy_strike:       // 痛击4
        case ItemType::Anger:              // 愤怒5
        case ItemType::Continuous_punches: // 连续拳6
        case ItemType::Observe_weaknesses: // 观察弱点7
        case ItemType::Activate_muscles:   // 活动肌肉8
        case ItemType::Revitalize_spirit:  // 重振精神9
        case ItemType::Metallization:      // 金属化10
        case ItemType::Unstoppable:        // 势不可挡11
        case ItemType::Rampart:            // 壁垒12
        case ItemType::Sacrifice:          // 祭品13
            std::cout << "Event : hitcard:" << event.val << std::endl;
            // 给玩家加卡牌
            player.AddCards(itemPileMap.at(itemtype));
            uiMgr.PushNotification("Obtain a card", itemTexMap.at(itemtype));
            break;
        default:
            break;
        }
    }
    break;
#pragma region 塔塔开
    // 开始战斗
    case EventType::BeginBattle:
        playerFaceBeforeBattle = player.GetFacing();
        playerXBeforeBattle = player.GetPos().x;
        btLogic.StartBattle(sceneMgr.GetScene().GetClickEnemy(), &player);
        sceneMgr.LoadScene(SceneType::Battle, [this]
                           {
            uiMgr.Get<CardsInHandPanel>()->SetCards(btLogic.getHandCardsPile(), btLogic.state.actionPoints, true);
            uiMgr.Open(PanelType::CardsInHand);
            player.SetFacing(1); });
        std::cout << "Event:beginBattle" << std::endl;
        break;
    // 玩家回合
    case EventType::PlayerTurn:
        // std::cout << "Event: start play" << std::endl;
        // std::cout << "抽牌前" << std::endl;
        btLogic.PilePre();                // 抽牌
        // std::cout << "抽牌后" << std::endl;
        // std::cout << "结算前" << std::endl;
        btLogic.PlayerStatusSettlement(); // 玩家状态结算
       // std::cout << "结算后" << std::endl;
        btLogic.ShowTurnCounts();
        //std::cout << "uiMgr.Getqian" << std::endl;
        uiMgr.Get<CardsInHandPanel>()->SetCards(btLogic.getHandCardsPile(), btLogic.state.actionPoints);
        //std::cout << "uiMgr.Gethou" << std::endl;

        break;
    // 结束玩家回合，开始敌人回合
    case EventType::EndTurn:
        std::cout << "Event: end turn" << std::endl;

        btLogic.turnsOver();
        uiMgr.Get<CardsInHandPanel>()->SetCards(btLogic.getHandCardsPile(), btLogic.state.actionPoints);
        btLogic.EnemyTurn();
        
        break;
    // 结束对局
    case EventType::EndBattle:
        if(btLogic.state.battleEnded) break;
        std::cout << "Event: EndBattle" << std::endl;
        btLogic.state.battleEnded = true;
        // 胜利
        if (event.val == 0)
        {
            sceneMgr.LoadGameBeforeBattle([this]
                                          {
                uiMgr.CloseAll();
                player.SetFacing(playerFaceBeforeBattle);
                player.SetFeet({playerXBeforeBattle, PlayerGroundY});
                player.ResetToStand();
        });
        }
        // 失败
        else if (event.val == 1)
        {
            ghostInfo.gameSceneID = sceneMgr.GetGameSceneIdx();
            ghostInfo.posX = playerXBeforeBattle;
            ghostInfo.keyNum = keyCnt;
            std::cout<<"deadcnt"<<playerDeadCnt<<std::endl;
            // 彻底死亡
            if(++playerDeadCnt > 1){
                uiMgr.CloseAll();
                sceneMgr.LoadScene(SceneType::Dead);
            }
            // 第一次死亡
            else{
                uiMgr.CloseAll();
                // 添加敌人魂
                sceneMgr.LoadFirstDie([this]{
                    keyCnt = 0;
                    sceneMgr.AddGhost(ghostInfo.gameSceneID, {ghostInfo.posX, PlayerGroundY - 300}, ghostInfo.keyNum);
                    player.ResetExceptCard();
                });
            }
        }
        // 打败魂
        else if (event.val == 2)
        {
            playerDeadCnt = 0;
            sceneMgr.LoadGameBeforeBattle([this]
                                          {
                uiMgr.Close(PanelType::CardsInHand);
                player.SetFacing(playerFaceBeforeBattle);
                player.SetFeet({playerXBeforeBattle, PlayerGroundY});
                player.ResetToStand();
            });
        }
        break;
    default:
        break;
    }
}

void Game::Draw()
{
    sf::Clock c;
    window.clear();

    SceneType curSceneType = sceneMgr.GetCurSceneType();

    // 绘制场景
    renderer.DrawScene(window, sceneMgr.GetScene());

    if (curSceneType == SceneType::Game || curSceneType == SceneType::Battle)
    {
        bool f = curSceneType == SceneType::Battle;
        renderer.DrawPlayer(window, player, f);
    }

    if (curSceneType == SceneType::Game)
    {
        renderer.DrawDialog(window, textHintMgr);
        for (auto &a : uiMgr.rewardAni.cards)
        {
            renderer.DrawCard(window, a, a.alpha);
        }
        if (textHintMgr.IsMovementHintVisible())
        {
            renderer.DrawMovementHint(window);
        }
        if (textHintMgr.IsDoorHintVisible())
        {
            renderer.DrawCenteredText(
                window,
                textHintMgr.GetDoorHintText(),
                textHintMgr.GetDoorHintAlpha());
        }
    }
    
    if (curSceneType == SceneType::Battle)
    {
        int column = 0;
        float space = 50;
        // 敌人意图
        const Enemy *e = sceneMgr.GetScene().GetClickEnemy();
        int num = e->allPlans[btLogic.state.TurnCount - 1].data;
        PlanType type = e->allPlans[btLogic.state.TurnCount - 1].plantype;
        sf::Vector2f pos = e->position;
        pos.x += 70;
        pos.y -= 30;
        renderer.DrawBuffWithNum(window, planTexMap.at(type), num, pos);
        // 敌人buff
        auto bounds = e->bound;
        pos.x = bounds.position.x + 10;
        pos.y = e->position.y + e->bound.size.y + e->HPDrawOffset + 40; // SFML 3
        auto buffs = btLogic.enemy->buff_debuff_vec.getBuffTypes();
        for(auto& buff : buffs){
            renderer.DrawBuff(window, buffTexMap.at(buff), {pos.x + column * space, pos.y}
                                , {40, 40});
            column++;
        }

        // 玩家Buff
        pos = player.feet;
        pos.x -= 110;
        pos.y += 50;
        column = 0;
        // 玩家防御
        if (btLogic.state.defend_num > 0)
        {
            int num = btLogic.state.defend_num;
            renderer.DrawBuffWithNum(window, TextureType::p_defend_player, num, 
                {player.feet.x - 110, player.feet.y - 30}, {50, 50}, 40, 30);
        }

        buffs = btLogic.state.buff_debuff_vec.getBuffTypes();
        for(auto& buff : buffs){
            renderer.DrawBuff(window, buffTexMap.at(buff), {pos.x + column * space, pos.y}, 
                                {40, 40});
            column++;
        }
    }

    // UI面板
    uiMgr.DrawPanels(window);
    // 捡到物品提示
    uiMgr.DrawNotifications(window);
    // 文字弹幕
    for (auto &p : uiMgr.textPrompt.prompts)
    {
        renderer.DrawText(window, p->text, p->position, p->size, p->alpha);
    }

    // 切场景遮罩
    if (sceneMgr.GetFadeAlpha() > 0.f)
    {
        renderer.DrawFadeOverlay(window, static_cast<std::uint8_t>(sceneMgr.GetFadeAlpha()));
    }

    window.display();
    // std::cout << "frame: " << c.restart().asMilliseconds() << "\n";
}
