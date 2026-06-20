#pragma once

#include "Constants.h"
#include "UI/TextHintManager.h"
#include "Enemy.h"
#include "Tools/ResourceManager.h"
#include "Player.h"
#include "UI/UIManager.h"
#include "Scene/Scene.h"
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
    CardView MakeFanCard( PileType type, int index, int count, sf::Vector2f center, float yOffset, float spacingX, float angleRange){
        CardView cv;
        cv.texType = cardTexMap.at(type);

        float mid = (count - 1) / 2.f;
        float offset = index - mid;

        float t = (count <= 1) ? 0.f : offset / mid;

        cv.basePosition = {
            center.x + offset * spacingX,
            center.y + (t * t) * 25.f + yOffset
        };

        cv.rotation = t * angleRange;

        return cv;
    }

    public:
    Renderer(ResourceManager& rm, UIManager& ui);
    ~Renderer();

    void Init();
    
    // Main drawing methods
    void DrawCard(sf::RenderWindow& window, const CardView& card, float alpha);
    void DrawPlayer(sf::RenderWindow& window, const Player& player);
    void DrawScene(sf::RenderWindow& window, Scene& scene);
    void DrawFadeOverlay(sf::RenderWindow& window, std::uint8_t alpha);
    void DrawItem(sf::RenderWindow& window, sf::Vector2f position, const TextureType& type
        , sf::Vector2f scale = {1.f, 1.f}, int index = 0);

    void DrawItemWithNum(sf::RenderWindow& window, TextureType type, int num, sf::Vector2f pos);
    
    void DrawDialog(sf::RenderWindow& window, const TextHintManager& textHintMgr);
    void DrawMovementHint(sf::RenderWindow& window);
    void DrawCenteredText(sf::RenderWindow& window, const std::string& text, float alpha);
void DrawText(
    sf::RenderWindow& window,
    const std::string& text,
    sf::Vector2f position,
    unsigned size,
    float alpha);
};