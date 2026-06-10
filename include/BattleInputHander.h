// BattleInputHandler.h
#pragma once
#include "BattleLogic.h"
#include "BattleRender.h"
#include "Input.h"
class BattleInputHandler {
public:
    void HandleInput(sf::RenderWindow& window, BattleLogic& logic, BattleRender& render);
    // 内部检测鼠标位置，调用 logic.SelectCard / logic.SelectTarget / logic.CancelSelection 等
};