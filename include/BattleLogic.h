#pragma once

#include "BuffAndDebuff.h"
#include "Card.h"
#include "Constants.h"
#include "Enemy.h"
#include "Player.h"
#include "UI/TextPromptManager.h"
#include "Tools/AudioManager.h"

#include <iostream>
#include <random>
#include <vector>

class TextPromptManager;

int getRandomInt(int min, int max);

struct BattleState
{
    bool battleEnded = false; // 战斗结束
    int cardSum = 0;

    int actionPoints = 0;
    int maxActionPoints = 3;
    int dealNums = 5;

    std::vector<Card> dealPile;
    std::vector<Card> handCards;
    std::vector<Card> discardPile;
    std::vector<Card> exhaustPile;

    int TurnCount = 0;
    bool isPlayerTurn = true;

    int defend_num = 0;
    int thornsdata = 0; // 被攻击时造成的反伤
    Buff_Debuff_Vec buff_debuff_vec;

    // Persistent and turn-scoped combat effects.
    int strength = 0;
    int temporaryStrength = 0;
    int rampageBonus = 0; // 暴走累计（暴走每被打出一次，本次战斗中伤害增加5）
    int metallicize = 0;
    int juggernautDamage = 0;
    bool barricade = false;

    int hoveredCardIndex = -1;
    int selectedCardIndex = -1;
    Card *pendingCard = nullptr;
    bool cardIsUsed = false;

    int hoveredPotionIndex = -1;
    int selectedPotionIndex = -1;
    bool potionIsUsed = false;

    int final_damage = -1; // 实际对玩家造成的血量下降
};

class BattleLogic
{
private:
    bool SpendActionPoints(int cost);
    void DiscardPlayedCard(int idx);
    bool IsAttackCard(PileType type) const;
    bool EnemyIntendsAttack() const;
    bool HasStatus(const std::vector<BDinfo> buff_debuffs, BuffDebuffType type);
    void DealDamage(Enemy *enemy, int baseDamage, int hitCount = 1, bool applyStrength = true);
    void DealDamage(int baseDamage, Enemy *enemy, bool applyStrength = true);
    void DealDirectDamage(Enemy *enemy, int damage);
    void DealDirectDamage(int damage);
    void GainBlock(int amount, Enemy *enemy);
    void sufferThorns(Enemy *enemy, int damage);
    void sufferThorns(Player *p, int damage);

    void UpdateHP(Player *player, int add);
    void UpdateHP(Enemy *enemy, int add);

public:
    BattleState state;
    std::vector<GameEvent> events;
    TextPromptManager *textPrompt;
    Player *player;
    Enemy *enemy;

    bool isWaitingForPlayerTurn = false;
    sf::Clock playerTurnDelayClock;
    const float PLAYER_TURN_DELAY_MS = 1000.0f; // 1000ms = 1秒
    void Update();                             // 与时间有关的更新
    bool IsWaitingForPlayerTurn() const { return isWaitingForPlayerTurn; }

    void StartBattle(Enemy *e, Player *p);
    bool BattleFinished();

    void PilePre();
    void TakePile(int n);
    void PlayerStatusSettlement();
    void ClickPlayer(int idx);
    void ClickEnemy(int idx);
    void turnsOver();
    void EnemyTurn();
    void ShowTurnCounts();
    void ShowEnemyDamage(Enemy &enemy);

    std::vector<PileType> getHandCardsPile();
    std::vector<PileType> getdisCardPile();
    std::vector<PileType> getdealCardsPile();

    // 敌人意图，敌人意图数字，敌人buff, 玩家buff, 玩家防御值
    std::tuple<PlanType, int, std::vector<std::pair<BuffDebuffType, int>>, std::vector<std::pair<BuffDebuffType, int>>, int>
    GetBuffInfo()
    {
        // 1. 敌人意图
        PlanType intent = PlanType::None;
        int intentNum = 0;

        std::vector<std::pair<BuffDebuffType, int>> enemyBuff;
        std::vector<std::pair<BuffDebuffType, int>> playerBuff;

        if (enemy)
        {
            int idx = state.TurnCount - 1;

            if (idx >= 0 && idx < (int)enemy->allPlans.size())
            {
                intent = enemy->allPlans[idx].plantype;
                intentNum = enemy->allPlans[idx].data;
            }

            // 敌人 buff
            enemyBuff = enemy->buff_debuff_vec.getBuffs();
        }

        // 2. 玩家 buff
        playerBuff = state.buff_debuff_vec.getBuffs();

        // 3. 防御值
        int defend = state.defend_num;

        return std::make_tuple(intent, intentNum, enemyBuff, playerBuff, defend);
    }
};
