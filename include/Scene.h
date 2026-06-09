#pragma once

#include "Constants.h"
#include <vector>
#include <SFML/Graphics.hpp>

// Button with bounds for click detection
struct Button {
    sf::Sprite sprite;
    sf::FloatRect bounds;
};

// Item class for UI elements
class Item {
private:
    sf::Sprite sprite;
    TextureType textureType;
    bool hovered = false;
    bool enabled = true;

public:
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    bool isClicked(sf::Vector2f mousePos) const {
        return enabled &&
               sprite.getGlobalBounds().contains(
                   static_cast<sf::Vector2f>(mousePos)
               );
    }

    void SetSprite(const sf::Sprite& s) { sprite = s; }
    sf::Sprite& GetSprite() { return sprite; }
};

// Scene class managing game scenes and carriage system
class Scene {
private:
    SceneType currentScene = SceneType::Menu;
    SceneType pendingScene = SceneType::Menu;
    
    // Carriage/Train system
    int currentCarriage = 0;      // Current carriage index (0-3)
    int pendingCarriage = 0;      // Next carriage to transition to
    
    // Carriage boundaries and entry/exit positions
    static constexpr float LeftExitX = 210.f;      // Player exit on left
    static constexpr float RightExitX = 1710.f;    // Player exit on right
    static constexpr float LeftEntryX = 275.f;     // Entry position from left
    static constexpr float RightEntryX = 1645.f;   // Entry position from right
    static constexpr int MaxCarriages = 4;         // Number of carriages (0-3)

public:
    Scene();

    // Scene management
    SceneType GetCurrentScene() const { return currentScene; }
    SceneType GetPendingScene() const { return pendingScene; }
    void SetCurrentScene(SceneType scene) { currentScene = scene; }
    void SetPendingScene(SceneType scene) { pendingScene = scene; }

    // Carriage management
    int GetCurrentCarriage() const { return currentCarriage; }
    int GetPendingCarriage() const { return pendingCarriage; }
    void SetCurrentCarriage(int carriage) { currentCarriage = carriage; }
    void SetPendingCarriage(int carriage) { pendingCarriage = carriage; }
    bool CanGoLeft() const { return currentCarriage > 0; }
    bool CanGoRight() const { return currentCarriage < MaxCarriages - 1; }

    // Position constants for carriage transitions
    static float GetLeftExitX() { return LeftExitX; }
    static float GetRightExitX() { return RightExitX; }
    static float GetLeftEntryX() { return LeftEntryX; }
    static float GetRightEntryX() { return RightEntryX; }
};