#pragma once

#include "Constants.h"
#include "ResourceManager.h"
#include "Player.h"
#include "UIManager.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

using namespace sf;

class Renderer {
private:
    ResourceManager& rm;
    UIManager& uiManager;
    
    // Background sprites - using pointers to avoid default constructor issue
    sf::Sprite* menuBackground = nullptr;
    sf::Sprite* gameBackground = nullptr;
    sf::Sprite* title = nullptr;

    // Fade overlay
    sf::RectangleShape fadeOverlay;

    // Window dimensions
    unsigned int windowWidth = 1920;
    unsigned int windowHeight = 1080;

    // Helper methods
    sf::Sprite makeSprite(const sf::Texture& texture, sf::Vector2f position);
    void scaleToWindow(sf::Sprite& sprite);
    //Button makeCenteredButton(const sf::Texture& texture, float centerY);

public:
    Renderer(ResourceManager& rm, UIManager& ui);
    ~Renderer();

    void Init();
    
    // Main drawing methods
    void DrawMenu(sf::RenderWindow& window);
    void DrawGame(sf::RenderWindow& window, const Player& player);
    void DrawSettings(sf::RenderWindow& window);
    void DrawFadeOverlay(sf::RenderWindow& window, std::uint8_t alpha);
    void DrawItem(sf::RenderWindow& window, sf::Vector2f position, const TextureType& type
        , sf::Vector2f scale = {1.f, 1.f}, int index = 0);

    // Window size
    unsigned int GetWindowWidth() const { return windowWidth; }
    unsigned int GetWindowHeight() const { return windowHeight; }
    void SetWindowSize(unsigned int w, unsigned int h) {
        windowWidth = w;
        windowHeight = h;
    }
};