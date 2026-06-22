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
    void DrawPlayer(sf::RenderWindow& window, const Player& player, bool inBattle, int key);
    void DrawScene(sf::RenderWindow& window, Scene& scene);
    void DrawFadeOverlay(sf::RenderWindow& window, std::uint8_t alpha);
    void DrawBuff(sf::RenderWindow& window, const TextureType& type, sf::Vector2f position, 
                    sf::Vector2f scope = {65, 65});

    void DrawBuffWithNum(sf::RenderWindow& window, TextureType type, int num, sf::Vector2f pos,
                    sf::Vector2f scope = {65, 65}, int space = 50, int fontsize = 30);
    
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