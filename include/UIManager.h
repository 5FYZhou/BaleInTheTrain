#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "ResourceManager.h"

class UIManager {
private:
    // Sprites - using pointers
    sf::Sprite* statusBox = nullptr;
    sf::Sprite* potionIcon1 = nullptr;
    sf::Sprite* potionIcon2 = nullptr;
    sf::Sprite* potionIcon3 = nullptr;
    sf::Sprite* cubeIcon = nullptr;
    sf::Sprite* settingsIcon = nullptr;

    // HP system
    sf::RectangleShape hpBack, hpBar;
    sf::Text* hpText = nullptr;
    int currentHP = 100;
    int maxHP = 100;
    bool hasFont = false;
    sf::Font font;

    // Window size
    float windowWidth = 1920.f;
    float windowHeight = 1080.f;

public:
    UIManager();
    ~UIManager();

    // Initialization
    bool Init(ResourceManager& rm);
    bool LoadFont();

    // HP management
    void SetHP(int current, int max);
    void TakeDamage(int damage);
    void Heal(int amount);
    int GetCurrentHP() const { return currentHP; }
    int GetMaxHP() const { return maxHP; }

    // Drawing
    void Draw(sf::RenderWindow& window);

    // Getters for UI elements
    sf::Sprite* GetStatusBox() const { return statusBox; }
    sf::Sprite* GetSettingsIcon() const { return settingsIcon; }
};
