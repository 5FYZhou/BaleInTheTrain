#pragma once
#include "Constants.h"
#include "BattleUI.h"
#include "Enemy.h"
#include "Card.h"
#include "Player.h"
#include "Input.h"
#include <random>
int getRandomInt(int min, int max); //生成随机数
struct BattleState {
    //输赢状态
    bool isWin = false;
    bool isLose = false;

    int playerHP, maxHP;    // 玩家血量
    //std::vector<Potion> potions; //药水
    //std::vector<Relic> relics;    //遗物

    int cardSum; //卡牌总数

    std::vector<Enemy> enemies;  // 当前场上敌人状态

    int actionPoints, maxActionPoints;  //行动点

    int dealPileCount; //当前抽牌堆数
    int dealNums = 5; //每回合开始抽牌数, 默认是5
    int discardPileCount; //当前弃牌堆数
    std::vector<Card> dealPile; //抽牌堆
    std::vector<Card> handCards;    //手牌
    std::vector<Card> discardPile;  //弃牌堆
    
    int TurnCount; //回合数
    bool isPlayerTurn; //是否玩家回合

    ////////// 临时交互状态
    /*卡牌*/
    int hoveredCardIndex = -1; // 当前悬停的手牌索引，-1表示没有悬停
    int selectedCardIndex = -1; // 当前选中手牌索引，-1表示未选中
    Card* pendingCard;          // 正在等待选目标的卡牌
    bool cardIsUsed;    //选中的卡牌被使用
    /*药水*/
    int hoveredPotionIndex = -1; // 当前悬停药水索引，-1表示未悬停
    int selectedPotionIndex = -1; // 当前选中药水索引，-1表示未选中
    //Potion* pendingPotion;      // 正在等待选目标的药水
    bool potionIsUsed;
};

class BattleLogic {
private:
    BattleState state;
public:
    void BattleLogicManager(const std::vector<Enemy>& initialEnemies, std::vector<Card>& Cards,  Player& player);
    void HandleInput(const sf::Vector2f& mousePos);
    void BattleUpdate();
    void turnsOver();
    void PlayerStatusSettlement(Player& player);//玩家状态结算
    void waitPlayerInput(int idx, Player& player);
    void waitPlayerInput(int idx, Enemy& enemy);

    bool BattleFinished(Player &player);
    void StartBattle(const std::vector<Enemy>& initialEnemies, const std::vector<Card>& Cards,  Player& player);
    void EndPlayerTurn();       // 敌人行动，然后切回玩家
    void SelectCard(int handIndex);   // 点击手牌时调用，可能进入等待目标状态
    void SelectTarget(sf::Vector2i mousePos); // 多目标时根据点击确定目标
    void CancelSelection();     // 右键取消
    BattleState& GetState();    // 供Render读取
    // ... 抽牌、弃牌、洗牌、伤害计算等内部方法
};