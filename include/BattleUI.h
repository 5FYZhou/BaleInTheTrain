// BattleUI.h
#pragma once

#include <SFML/Graphics.hpp>

class BattleUI {
public:
    // 所有坐标、间距、大小硬编码或从配置读取
    sf::Vector2f drawPilePos;   // 抽牌池位置
    sf::Vector2f discardPilePos;
    sf::Vector2f actionPointPos;
    sf::Rect<float> handArea;   // 手牌区域（用于动态排列）
    float cardWidth, cardHeight;
    float cardHoverOffset;      // 悬停上浮像素
    // ... 其他
};

