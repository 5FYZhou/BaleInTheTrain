#pragma once

#include "Constants.h"
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

struct Item{
    sf::Vector2f pos;
    ItemType type;
};

inline const std::vector<std::vector<Item>> itemInScene = {
    { {{100.f, 100.f}, ItemType::Strike}, {{200.f, 100.f}, ItemType::Strike} },
    { {{100.f, 200.f}, ItemType::Strike}, {{200.f, 200.f}, ItemType::Strike}, {{150.f, 300.f}, ItemType::Defend} },
    { {{100.f, 300.f}, ItemType::Strike}, {{200.f, 300.f}, ItemType::Strike}, {{150.f, 400.f}, ItemType::Defend}, {{150.f, 200.f}, ItemType::Key} }
};

inline const std::vector<std::tuple<int, sf::Vector2f, sf::Vector2f>> enemyInScene = {
    { 0, { 1500, 500 }, { 213, 377 } },
    { 1, { 1500, 500 }, { 184, 376 } },
    { 2, { 1500, 500 }, { 176, 294 } }
};

struct SceneInteractable {
    bool clickable = true;
    TextureType texture;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Vector2f scale;
    sf::FloatRect bounds = sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    EventType eventType = EventType::None;
    ItemType itemType = ItemType::Button;
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
    TextureType bgTex;
    std::vector<SceneInteractable> interactables;
public:
    Scene(std::vector<GameEvent>& e) : events(e) {}
    virtual ~Scene() = default;

    virtual void Enter() {}
    virtual void Leave() {}

    virtual void BuildInteractables() = 0;
    const std::vector<SceneInteractable>& GetInteractables() const {
        return interactables;
    }
    SceneType GetType() const { return type; }
    TextureType GetBgTextrue() const { return bgTex; }

    virtual void ProcessInput(const sf::Vector2f& mousePos) = 0;

    virtual void Update(float dt) {}
};

class MenuScene : public Scene {
private:
public:
    MenuScene(std::vector<GameEvent>& e) : Scene(e) { 
        type = SceneType::Menu; 
        bgTex = TextureType::MenuBackground;
    }
    void BuildInteractables() override {
        interactables.clear();
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
    int enemyIdx;
    sf::Vector2f enemyPos;
    sf::FloatRect enemyCollider;
public:
    GameScene(std::vector<GameEvent>& e, int i) : Scene(e), idx(i) { 
        type = SceneType::Game; 
        bgTex = TextureType::GameBackground;
    }
    void BuildInteractables() override {
        interactables.clear();
        for (const auto& item : itemInScene[idx]) {
            interactables.push_back({ TextureType::Star, item.pos, {90, 109}, EventType::ItemClicked, item.type });
        }
        interactables.push_back({TextureType::BackpackIcon, {1695.f, 25.f}, {179, 190}, {0.45f, 0.45f}, EventType::OpenBackpackIcon});
        interactables.push_back({TextureType::SettingsIcon, {1800.f, 30.f}, {100, 107}, {0.68f, 0.68f}, EventType::OpenSettings});

        enemyIdx = std::get<0>(enemyInScene[idx]);
        enemyPos = std::get<1>(enemyInScene[idx]);
        sf::Vector2f enemySize = std::get<2>(enemyInScene[idx]);
        enemyCollider = sf::FloatRect(enemyPos, enemySize);
        interactables.push_back({enemyTexMap.at(static_cast<EnemyType>(enemyIdx)),
             enemyPos, enemySize, EventType::None});
    }

    void ProcessInput(const sf::Vector2f& mousePos) override {
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
};

class BattleScene : public Scene {
public:
    BattleScene(std::vector<GameEvent>& e, int i) : Scene(e) { 
        type = SceneType::Battle; 
        bgTex = TextureType::GameBackground;
    }
    void BuildInteractables() override {
        interactables.clear();
    }

    void ProcessInput(const sf::Vector2f& mousePos) override {
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
};

