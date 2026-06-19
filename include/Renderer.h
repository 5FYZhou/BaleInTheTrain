#pragma once

#include "Constants.h"
#include "TextHintManager.h"
#include "Enemy.h"
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
    const sf::Font* font;
    
    // player HP
    sf::Sprite* statusBox = nullptr;
    sf::RectangleShape hpBack;
    sf::RectangleShape hpBar;
    sf::Text* hpText = nullptr;

    // UI
    sf::Sprite* dialogBox = nullptr;
    sf::Text* dialogText = nullptr;
    sf::Text* dialogHintText = nullptr;
    sf::Text* movementHintText = nullptr;

    // Fade overlay
    sf::RectangleShape fadeOverlay;

    // Helper methods
    sf::Sprite makeSprite(const sf::Texture& texture, sf::Vector2f position);
    //Button makeCenteredButton(const sf::Texture& texture, float centerY);
    void DrawCard(sf::RenderWindow& window, const CardView& card, float alpha);

public:
    Renderer(ResourceManager& rm, UIManager& ui);
    ~Renderer();

    void Init();
    
    // Main drawing methods
    void DrawPlayer(sf::RenderWindow& window, const Player& player);
    void DrawScene(sf::RenderWindow& window, const Scene& scene);
    void DrawFadeOverlay(sf::RenderWindow& window, std::uint8_t alpha);
    void DrawItem(sf::RenderWindow& window, sf::Vector2f position, const TextureType& type
        , sf::Vector2f scale = {1.f, 1.f}, int index = 0);

    void DrawEnemyPlan(sf::RenderWindow& window, PlanType type, int num, sf::Vector2f pos);
    
    void DrawDialog(sf::RenderWindow& window, const TextHintManager& textHintMgr);
    void DrawMovementHint(sf::RenderWindow& window);
    void DrawCardRewards(sf::RenderWindow& window, const std::vector<PileType>& cardsV, float alpha);
    void DrawCenteredText(sf::RenderWindow& window, const std::string& text, float alpha);
};