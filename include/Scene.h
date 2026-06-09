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
    { {{100.f, 100.f}, ItemType::Attack}, {{200.f, 100.f}, ItemType::Attack} },
    { {{100.f, 200.f}, ItemType::Attack}, {{200.f, 200.f}, ItemType::Attack}, {{150.f, 300.f}, ItemType::Heal} },
    { {{100.f, 300.f}, ItemType::Attack}, {{200.f, 300.f}, ItemType::Attack}, {{150.f, 400.f}, ItemType::Heal}, {{150.f, 200.f}, ItemType::Buff} }
};

struct SceneInteractable {
    TextureType texture;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::FloatRect bounds = sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    EventType eventType = EventType::None;
    ItemType itemType = ItemType::Button;
    SceneInteractable(TextureType tex, sf::Vector2f pos, sf::Vector2f sz, EventType e, ItemType i = ItemType::Button)
        : texture(tex), position(pos), size(sz), eventType(e), itemType(i) {
        bounds = sf::FloatRect(position, size);
        }
};

class Scene {
protected:
    std::vector<GameEvent>& events;
    SceneType type;
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

    virtual void ProcessInput(const sf::Vector2f& mousePos) = 0;

    virtual void Update(float dt) {}
};

class MenuScene : public Scene {
private:
public:
    MenuScene(std::vector<GameEvent>& e) : Scene(e) { type = SceneType::Menu; }
    void BuildInteractables() override {
        interactables.clear();
        interactables = {
            { TextureType::StartButton, {704.f, 510.f}, {512.f, 159.f}, EventType::StartGame },
            { TextureType::SettingsButton, {704.f, 665.f}, {512.f, 159.f}, EventType::OpenSettings },
            { TextureType::ExitButton, {704.f, 820.f}, {512.f, 159.f}, EventType::ExitGame }
        };
    }
    void ProcessInput(const sf::Vector2f& mousePos) override {
        for (const auto& item : interactables) {
            if (item.eventType == EventType::None) {
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
public:
    GameScene(std::vector<GameEvent>& e, int i) : Scene(e), idx(i) { type = SceneType::Game; }
    void BuildInteractables() override {
        interactables.clear();
        for (const auto& item : itemInScene[idx]) {
            interactables.push_back({ TextureType::Star, item.pos, {90, 109}, EventType::ItemClicked, item.type });
        }
    }
    void ProcessInput(const sf::Vector2f& mousePos) override {
        for (size_t i = 0; i < interactables.size(); ++i) {
            const auto& item = interactables[i];
            if (item.eventType == EventType::None) {
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

