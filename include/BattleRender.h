// BattleRender.h
#pragma once

#include "ResourceManager.h"
#include "BattleUI.h"
#include "BattleLogic.h"
#include "Card.h"
#include "Enemy.h"
#include "Player.h"


class BattleRender {
public:
    BattleRender(ResourceManager& rm);
    void Draw(sf::RenderWindow& window);  // 每帧调用，从BattleLogic读取状态绘制
private:
    int hoveredCardIndex = -1; // 当前悬停的手牌索引，-1表示没有悬停
    sf::Font font;
    BattleUI layout;  // 存储所有UI元素的坐标、尺寸
    ResourceManager& rm;
    sf::Sprite cardSprite, iconSprite; // 等
    void Init();
    void DrawHandCards(sf::RenderWindow& window,const BattleState& state);   // 根据手牌数据动态计算位置、绘制
    void DrawPile(PileType type, sf::Vector2f pos, int count);
    void DrawActionPoints(int current, int max);
    void DrawPlayerStatus();
    void DrawEnemy();
    void DrawArrow(sf::Vector2f from, sf::Vector2f to); // 鼠标到目标
    void DrawOutline(sf::Sprite& target); // 红色轮廓
};