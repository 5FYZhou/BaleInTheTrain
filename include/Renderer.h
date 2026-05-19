#pragma once

#include "Constants.h"
#include "ResourceManager.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
using namespace sf;

class Renderer {
private:
    ResourceManager& rm;
    int curGridSize;
    sf::Vector2f gridStartPos;
	std::optional<sf::Sprite>  sBackground, sTiles, sButtons, sNum, sTimer, sCounter, sGameOver;       //	创建精灵对象
public:
    Renderer(ResourceManager& rm, int& g);
    void Init();
    void DrawBackground(RenderWindow& window, int index = 0);
    void DrawGrid(RenderWindow& window, const Scene& grid);
    void DrawButton(RenderWindow& window, const Vector2i& LeftCorner, const int& detaX, const std::vector<Button_my>& btns);
    void DrawScore(RenderWindow& window, int score);
    void DrawTimer(RenderWindow& window, float time);
    void DrawGameEnd(RenderWindow& window, bool win);
};