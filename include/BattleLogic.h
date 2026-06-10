#pragma once
#include "Constants.h"
#include "BattleUI.h"
#include "Enemy.h"
#include "Card.h"
struct BattleState {
    int playerHP, maxHP;
    int actionPoints, maxActionPoints;
    std::vector<Card> drawPile;
    std::vector<Card> handCards;
    std::vector<Card> discardPile;
    std::vector<Potion> potions;
    std::vector<Relic> relics;
    std::vector<Enemy> enemies;
    bool isPlayerTurn;
    // 临时交互状态
    Card* pendingCard;          // 正在等待选目标的卡牌
    // ... 其他
};

class BattleLogic {
public:
    void StartBattle(/*初始敌人数据*/);
    void EndPlayerTurn();       // 敌人行动，然后切回玩家
    void PlayCard(Card* card, void* target = nullptr); // target 可为敌人或玩家自己
    void SelectCard(int handIndex);   // 点击手牌时调用，可能进入等待目标状态
    void SelectTarget(sf::Vector2i mousePos); // 多目标时根据点击确定目标
    void CancelSelection();     // 右键取消
    BattleState& GetState();    // 供Render读取
    // ... 抽牌、弃牌、洗牌、伤害计算等内部方法
};