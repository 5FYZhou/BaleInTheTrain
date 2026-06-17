#pragma once

#include "Constants.h"
#include "Enemy.h"
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <cmath>

struct Item{
    sf::Vector2f pos;
    ItemType type;
};

inline const std::vector<std::vector<Item>> itemInScene = {
    { {{100.f, 100.f}, ItemType::Activate_muscles}, {{200.f, 100.f}, ItemType::Strike} },
    { {{100.f, 200.f}, ItemType::Strike}, {{200.f, 200.f}, ItemType::Strike}, {{150.f, 300.f}, ItemType::Defend} },
    { {{100.f, 300.f}, ItemType::Strike}, {{200.f, 300.f}, ItemType::Strike}, {{150.f, 400.f}, ItemType::Defend}, {{150.f, 200.f}, ItemType::Key} }
};

inline const std::vector<std::pair<EnemyType, sf::Vector2f>> enemyInScene = {
    { EnemyType::Train_attendant, { 1500, 490 } },
    { EnemyType::LightMonster, { 1500, 480 } },
    { EnemyType::TicketMonster, { 1500, 560 } }
};

struct SceneInteractable {
    bool clickable = true; // 可点击的
    TextureType texture; // 纹理
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Vector2f scale;
    sf::FloatRect bounds = sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    EventType eventType = EventType::None; // 点击后发出的事件
    ItemType itemType = ItemType::Button; // 当前物品的类型
    SceneInteractable(bool c, TextureType tex, sf::Vector2f pos)
        : clickable(c), texture(tex), position(pos) {
        scale = {1.f, 1.f};
    }
    SceneInteractable(TextureType tex, sf::Vector2f pos, sf::Vector2f sz, EventType e, ItemType i = ItemType::Button)
        : texture(tex), position(pos), size(sz), eventType(e), itemType(i) {
        scale = {1.f, 1.f};
        UpdateBounds();
    }
    SceneInteractable(TextureType tex, sf::Vector2f pos, sf::Vector2f sz, sf::Vector2f sc, EventType e, ItemType i = ItemType::Button)
        : texture(tex), position(pos), size(sz), scale(sc), eventType(e), itemType(i) {
        UpdateBounds();
    }
    void UpdateBounds(){
        sf::Vector2f scaledSize = {
            size.x * scale.x,
            size.y * scale.y
        };
    bounds = sf::FloatRect(position, scaledSize);
    }
};

class Scene {
protected:
    std::vector<GameEvent>& events;
    SceneType type;
    TextureType bgTex = TextureType::None;
    std::vector<SceneInteractable> interactables;
public:
    Scene(std::vector<GameEvent>& e) : events(e) {}
    virtual ~Scene() = default;

    virtual void Enter() {}
    virtual void Leave() {}

    const std::vector<SceneInteractable>& GetInteractables() const { return interactables; }
    SceneType GetType() const { return type; }
    TextureType GetBgTextrue() const { return bgTex; }
    virtual const Enemy* GetEnemy() const{ return nullptr; }
    virtual Enemy* GetEnemy() { return nullptr; }

    virtual void EnemyDrop() { return; }

    virtual void ProcessInput(const sf::Vector2f& mousePos) = 0;

    virtual void Update(float dt) {}
};

class MenuScene : public Scene {
public:
    MenuScene(std::vector<GameEvent>& e) : Scene(e) { 
        type = SceneType::Menu; 
        bgTex = TextureType::MenuBackground;
        interactables = {
            { false, TextureType::Title, {580.f, 85.f}},
            { TextureType::StartButton, {704.f, 510.f}, {512.f, 159.f}, EventType::StartGame },
            { TextureType::SettingsButton, {704.f, 665.f}, {512.f, 159.f}, EventType::OpenSettings },
            { TextureType::ExitButton, {704.f, 820.f}, {512.f, 159.f}, EventType::ExitGame }
        };
    }
    void ProcessInput(const sf::Vector2f& mousePos) override {
        for (const auto& item : interactables) {
            if (!item.clickable || item.eventType == EventType::None) {
                continue;
            }
            if (item.bounds.contains(mousePos)) {
                ::GameEvent event;
                event.type = item.eventType;
                events.push_back(event);
            }
        }
    }
};

class GameScene : public Scene {
private:
    int idx;
    Enemy enemy;
public:
    GameScene(std::vector<GameEvent>& e, int i) : Scene(e), idx(i), 
     enemy(enemyInScene[idx].first, enemyInScene[idx].second) { 
        type = SceneType::Game; 
        bgTex = TextureType::GameBackground;
        interactables.clear();
        for (const auto& item : itemInScene[idx]) {
            interactables.push_back({ TextureType::Star, item.pos, {90, 109}, EventType::ItemClicked, item.type });
        }
        interactables.push_back({TextureType::BackpackIcon, {1695.f, 25.f}, {179, 190}, {0.45f, 0.45f}, EventType::OpenBackpackIcon});
        interactables.push_back({TextureType::SettingsIcon, {1800.f, 30.f}, {100, 107}, {0.68f, 0.68f}, EventType::OpenSettings});
    }
    
    const Enemy* GetEnemy() const override { return &enemy; }
    Enemy* GetEnemy() override { return &enemy; }

    void EnemyDrop() override {
        //enemy.dead = true;
        if(!enemy.dead) return;
        static std::random_device rd;
        static std::mt19937 gen(rd());

        constexpr float DROP_RADIUS = 50.f;

        std::uniform_real_distribution<float> angleDist(0.f, 2.f * 3.1415926f);
        std::uniform_real_distribution<float> radiusDist(0.f, DROP_RADIUS);

        for(auto& item : enemy.droppedItems){
            float angle = angleDist(gen);
            float radius = radiusDist(gen);

            sf::Vector2f dropPos = enemy.position;
            dropPos.x += std::cos(angle) * radius;
            dropPos.y += std::sin(angle) * radius;

            interactables.push_back({
                TextureType::Star,
                dropPos,
                {90, 109},
                EventType::ItemClicked,
                item
            });
        }
    }

    void ProcessInput(const sf::Vector2f& mousePos) override {
        if(!enemy.dead && enemy.bound.contains(mousePos)){
            GameEvent event;
            event.type = EventType::BeginBattle;
            event.val = static_cast<int>(enemy.ty);
            events.push_back(event);
            // 因为要进入战斗场景了，不再处理当前场景的点击
            return;
        }

        std::vector<SceneInteractable> n;
        for (size_t i = 0; i < interactables.size(); ++i) {
            const auto& item = interactables[i];
            if (!item.clickable || item.eventType == EventType::None) {
                continue;
            }
            if (item.bounds.contains(mousePos)) {
                ::GameEvent event;
                event.type = item.eventType;
                // 这里的val可以用来区分不同的物品，例如可以直接使用itemInScene中的类型枚举值
                event.val = static_cast<int>(item.itemType);
                events.push_back(event);
                if(item.itemType == ItemType::Button)
                    // 按钮不删除
                    n.push_back(item);
            }
            else{
                // 未被点击的不删除
                n.push_back(item);
            }
        }
        interactables = n;
    }

    void Update(float dt) override{
        enemy.Update(dt);
    }
};

class BattleScene : public Scene {
private:
    Enemy* enemy = nullptr;
public:
    BattleScene(std::vector<GameEvent>& e) : Scene(e) { 
        type = SceneType::Battle; 
        bgTex = TextureType::GameBackground;
        interactables = {
            { TextureType::Potion1, {365.f, 30.f}, {137.f, 132.f}, {0.72f, 0.72f}, EventType::None },
            { TextureType::Potion2, {450.f, 30.f}, {137.f, 132.f}, {0.72f, 0.72f}, EventType::None },
            { TextureType::Potion3, {535.f, 30.f}, {137.f, 132.f}, {0.72f, 0.72f}, EventType::None },
            { TextureType::Cube, {100.f, 900.f}, {137.f, 132.f}, {0.72f, 0.72f}, EventType::OpenDealCardPanel },
            { TextureType::DiscardPile, {1700.f, 900.f}, {193.f, 203.f}, {0.45f, 0.45f}, EventType::OpenDiscardPile },
            { TextureType::EndTurn, {1250, 750}, {157, 69}, EventType::EndTurn},
            { TextureType::None, {357, 508}, {195, 352}, {-1.f, 1.f}, EventType::ItemClicked, ItemType::Player}
        };
    }
    ~BattleScene(){ delete enemy; }

    void SetEnemy(Enemy* e) {
        if(!e){
            std::cout<<"BattleScene:nullptr set"<<std::endl;
            return;
        }
        enemy = e; 
    }
    const Enemy* GetEnemy() const override { return enemy; }

    void ProcessInput(const sf::Vector2f& mousePos) override {
        if(enemy->bound.contains(mousePos)){
            GameEvent event;
            event.type = EventType::ItemClicked;
            event.val = static_cast<int>(ItemType::Enemy);
            events.push_back(event);
        }

        for (size_t i = 0; i < interactables.size(); ++i) {
            const auto& item = interactables[i];
            if (!item.clickable || item.eventType == EventType::None) {
                continue;
            }
            if (item.bounds.contains(mousePos)) {
                ::GameEvent event;
                event.type = item.eventType;
                // 这里的val可以用来区分不同的物品，例如可以直接使用itemInScene中的类型枚举值
                event.val = static_cast<int>(item.itemType);
                events.push_back(event);
            }
        }
    }
    void Update(float dt) override{
        enemy->Update(dt);
    }
};

class WinScene : public Scene {
public:
    WinScene(std::vector<GameEvent>& e) : Scene(e) { 
        type = SceneType::Win; 
        interactables = {
            { false, TextureType::Win, {715.f, 349.f}}
        };
    }
    void ProcessInput(const sf::Vector2f& mousePos) override {
        GameEvent event;
        event.type = EventType::ReturnMenu;
        events.push_back(event);
    }
};

