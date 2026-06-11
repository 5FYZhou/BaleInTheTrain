#pragma once

#include "Constants.h"
#include "DialogManager.h"
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
    
    sf::Sprite* statusBox = nullptr;
    sf::RectangleShape hpBack;
    sf::RectangleShape hpBar;
    sf::Text* hpText = nullptr;

    // UI
    sf::Sprite* potionIcon1 = nullptr;
    sf::Sprite* potionIcon2 = nullptr;
    sf::Sprite* potionIcon3 = nullptr;
    sf::Sprite* cubeIcon = nullptr;
    sf::Sprite* discardPileIcon = nullptr;
    sf::Sprite* dialogBox = nullptr;

    sf::Text* dialogText = nullptr;
    sf::Text* dialogHintText = nullptr;
    sf::Text* movementHintText = nullptr;

    // Fade overlay
    sf::RectangleShape fadeOverlay;

    // Helper methods
    sf::Sprite makeSprite(const sf::Texture& texture, sf::Vector2f position);
    void scaleToWindow(sf::Sprite& sprite);
    //Button makeCenteredButton(const sf::Texture& texture, float centerY);
    void DrawCard(sf::RenderWindow& window, const CardView& card, float alpha);

public:
    Renderer(ResourceManager& rm, UIManager& ui);
    ~Renderer();

    void Init();
    
    // Main drawing methods
    void DrawPlayer(sf::RenderWindow& window, const Player& player);
    void DrawScene(sf::RenderWindow& window, const Scene& scene);
    void DrawUI(sf::RenderWindow& window);
    void DrawFadeOverlay(sf::RenderWindow& window, std::uint8_t alpha);
    void DrawItem(sf::RenderWindow& window, sf::Vector2f position, const TextureType& type
        , sf::Vector2f scale = {1.f, 1.f}, int index = 0);
    //void DrawEnemy(sf::RenderWindow& window, const Enemy& enemy);
    void DrawEnemy(sf::RenderWindow& window, const Enemy& enemy);
    
    void DisablePotionIcons();
    void EnablePotionIcon(int index);
    void ApplyDisabledIconColor(sf::Sprite* sprite);

    void DrawDialog(sf::RenderWindow& window, const DialogManager& dialogMgr);
    void DrawMovementHint(sf::RenderWindow& window);
    void DrawCardRewards(sf::RenderWindow& window, const std::vector<PileType>& cardsV, float alpha);
};