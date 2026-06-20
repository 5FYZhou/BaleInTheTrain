#pragma once

#include "BuffAndDebuff.h"
#include "Card.h"
#include "Constants.h"
#include "Enemy.h"
#include "Player.h"

#include <iostream>
#include <random>
#include <vector>

int getRandomInt(int min, int max);

struct BattleState {
    int playerHP = 0;
    int maxHP = 0;
    int cardSum = 0;

    std::vector<Enemy> enemies;

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
    Buff_Debuff_Vec buff_debuffs;

    // Persistent and turn-scoped combat effects.
    int strength = 0;
    int temporaryStrength = 0;
    int rampageBonus = 0;
    int metallicize = 0;
    int juggernautDamage = 0;
    bool barricade = false;

    int hoveredCardIndex = -1;
    int selectedCardIndex = -1;
    Card* pendingCard = nullptr;
    bool cardIsUsed = false;

    int hoveredPotionIndex = -1;
    int selectedPotionIndex = -1;
    bool potionIsUsed = false;
};

class BattleLogic {
private:
    bool SpendActionPoints(int cost);
    void DiscardPlayedCard(int idx);
    bool IsAttackCard(PileType type) const;
    bool EnemyIntendsAttack(const Enemy& enemy) const;
    bool EnemyHasStatus(const Enemy& enemy, BuffDebuffType type) const;
    void DealDamage(Enemy& enemy, int baseDamage, int hitCount = 1, bool applyStrength = true);
    void DealDirectDamage(Enemy& enemy, int damage);
    void GainBlock(int amount, Enemy* enemy);

public:
    BattleState state;
    std::vector<GameEvent> events;

    void StartBattle(const std::vector<Enemy>& initialEnemies,
                     const std::vector<Card>& cards,
                     Player& player);
    bool BattleFinished(std::vector<Enemy*> enemies);

    void PilePre();
    void TakePile(int n);
    void PlayerStatusSettlement();
    void waitPlayerInput(int idx, Player& player, Enemy* enemy = nullptr);
    void waitPlayerInput(int idx, Enemy& enemy);
    void turnsOver(Enemy* enemy = nullptr);
    void EnemyTurn(Player& player, Enemy& enemy);

    std::vector<PileType> getHandCardsPile();
    std::vector<PileType> getdisCardPile();
    std::vector<PileType> getdealCardsPile();
};
