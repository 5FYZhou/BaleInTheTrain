#pragma once

#include "Constants.h"
#include "ResourceManager.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
using namespace sf;

class Renderer {
private:
    ResourceManager& rm;
	std::optional<sf::Sprite>  sBackground, sTiles, sButtons, sNum, sTimer, sCounter, sGameOver;       //	创建精灵对象
public:
    Renderer(ResourceManager& rm);
    void Init();
};