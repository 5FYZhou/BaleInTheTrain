#pragma once

#include "Constants.h"
#include <vector>

class Item{
private:
    sf::Sprite sprite;
    TextureType textureType;

    bool hovered = false;
    bool enabled = true;
public:
    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite);
    }

    bool isClicked(sf::Vector2f mousePos) const
    {
        return enabled &&
               sprite.getGlobalBounds().contains(
                   static_cast<sf::Vector2f>(mousePos)
               );
    }
};

class Scene {
private:
    std::vector<std::vector<sf::Vector2f>> cardsInEachScene;
public:
    Scene();

    std::vector<sf::Vector2f> GetCardsInOneScene(int idx) const { return cardsInEachScene[idx]; }
};