#include "BattleLogic.h"
#include "UI/TextPromptManager.h"

#include <algorithm>

namespace
{

    void RecycleDiscardPile(BattleState &state)
    {
        if (state.discardPile.empty())
            return;

        state.dealPile.insert(
            state.dealPile.end(),
            state.discardPile.begin(),
            state.discardPile.end());
        state.discardPile.clear();
    }

} // namespace

bool BattleLogic::SpendActionPoints(int cost)
{
    if (state.actionPoints < cost)
    {
        std::cout << "行动点不足" << std::endl;
        return false;
    }

    state.actionPoints -= cost;
    return true;
}

void BattleLogic::DiscardPlayedCard(int idx)
{
    if (idx < 0 || idx >= static_cast<int>(state.handCards.size()))
        return;

    state.discardPile.push_back(state.handCards[idx]);
    state.handCards.erase(state.handCards.begin() + idx);
}

bool BattleLogic::IsAttackCard(PileType type) const
{
    switch (type)
    {
    case PileType::Strike:
    case PileType::Rage:
    case PileType::Heavy_strike:
    case PileType::Anger:
    case PileType::Continuous_punches:
        return true;
    default:
        return false;
    }
}

bool BattleLogic::EnemyIntendsAttack(const Enemy &enemy) const
{
    if (enemy.allPlans.empty())
        return false;

    const int planIndex = std::max(0, state.TurnCount - 1) % static_cast<int>(enemy.allPlans.size());
    return enemy.allPlans[planIndex].plantype == PlanType::attack;
}

void BattleLogic::DealDirectDamage(Enemy &enemy, int damage)
{
    damage = std::max(0, damage);
    const int blocked = std::min(enemy.defend_num, damage);
    enemy.defend_num -= blocked;
    damage -= blocked;
    enemy.cur_health = std::max(0, enemy.cur_health - damage);
}

void BattleLogic::DealDirectDamage(int damage)
{
    damage = std::max(0, damage);
    const int blocked = std::min(state.defend_num, damage);
    state.defend_num -= blocked;
    damage -= blocked;
    state.playerHP = std::max(0, state.playerHP - damage);
}

void BattleLogic::DealDamage(Enemy &enemy, int baseDamage, int hitCount, bool applyStrength)
{
    for (int hit = 0; hit < hitCount && enemy.cur_health > 0; ++hit)
    {
        int damage = baseDamage;
        if (applyStrength)
            damage += state.strength + state.temporaryStrength;

        if (HasStatus(enemy.buff_debuff_vec.buff_debuffs, BuffDebuffType::easy_to_attack))
            damage = static_cast<int>(damage * 1.5);

        if (HasStatus(state.buff_debuff_vec.buff_debuffs, BuffDebuffType::vulnerable))
            damage = static_cast<int>(damage * 0.75);

        DealDirectDamage(enemy, damage);
    }
}

void BattleLogic::DealDamage(int baseDamage, Enemy &enemy, bool applyStrength)
{
    int damage = baseDamage;
    if (applyStrength)
        damage += enemy.strength;

    if (HasStatus(state.buff_debuff_vec.buff_debuffs, BuffDebuffType::easy_to_attack))
        damage = static_cast<int>(damage * 1.5);

    if (HasStatus(enemy.buff_debuff_vec.buff_debuffs, BuffDebuffType::vulnerable))
        damage = static_cast<int>(damage * 0.75);

    DealDirectDamage(damage);
}
void BattleLogic::GainBlock(int amount, Enemy *enemy)
{
    if (amount <= 0)
        return;

    state.defend_num += amount;

    if (enemy && enemy->cur_health > 0 && state.juggernautDamage > 0)
        DealDirectDamage(*enemy, state.juggernautDamage);
}

void BattleLogic::PilePre()
{
    state.TurnCount += 1;
    state.actionPoints = state.maxActionPoints;
    TakePile(state.dealNums);
}

void BattleLogic::TakePile(int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (state.dealPile.empty())
            RecycleDiscardPile(state);

        if (state.dealPile.empty())
            return;

        const int randomIndex = getRandomInt(0, static_cast<int>(state.dealPile.size()) - 1);
        state.handCards.push_back(state.dealPile[randomIndex]);
        state.dealPile.erase(state.dealPile.begin() + randomIndex);
    }
}

void BattleLogic::turnsOver(Enemy *enemy)
{
    if (state.metallicize > 0)
        GainBlock(state.metallicize, enemy);

    state.isPlayerTurn = false;
    state.discardPile.insert(
        state.discardPile.end(),
        state.handCards.begin(),
        state.handCards.end());
    state.handCards.clear();
}

void BattleLogic::PlayerStatusSettlement()
{
    state.temporaryStrength = 0;
    if (!state.barricade)
        state.defend_num = 0;

    state.buff_debuff_vec.update();
}

void BattleLogic::waitPlayerInput(int idx, Player &player, Enemy *enemy)
{
    if (idx < 0 || idx >= static_cast<int>(state.handCards.size()))
        return;

    const PileType type = state.handCards[idx].name;

    switch (type)
    {
    case PileType::Defend:
        if (!SpendActionPoints(1))
            return;
        GainBlock(5, enemy);
        DiscardPlayedCard(idx);
        break;

    case PileType::Shrug_off:
        if (!SpendActionPoints(1))
            return;
        GainBlock(8, enemy);
        TakePile(1);
        DiscardPlayedCard(idx);
        break;

    case PileType::Activate_muscles:
        state.temporaryStrength += 2;
        DiscardPlayedCard(idx);
        break;

    case PileType::Revitalize_spirit:
    {
        if (!SpendActionPoints(1))
            return;

        int exhaustedCount = 0;
        for (int i = static_cast<int>(state.handCards.size()) - 1; i >= 0; --i)
        {
            if (i == idx || IsAttackCard(state.handCards[i].name))
                continue;

            state.exhaustPile.push_back(state.handCards[i]);
            state.handCards.erase(state.handCards.begin() + i);
            ++exhaustedCount;
            if (i < idx)
                --idx;
        }

        GainBlock(exhaustedCount * 5, enemy);
        DiscardPlayedCard(idx);
        break;
    }

    case PileType::Metallization:
        if (!SpendActionPoints(1))
            return;
        state.metallicize += 3;
        DiscardPlayedCard(idx);
        break;

    case PileType::Unstoppable:
        if (!SpendActionPoints(2))
            return;
        state.juggernautDamage += 5;
        DiscardPlayedCard(idx);
        break;

    case PileType::Rampart:
        if (!SpendActionPoints(3))
            return;
        state.barricade = true;
        DiscardPlayedCard(idx);
        break;

    case PileType::Sacrifice:
        state.playerHP = std::max(0, state.playerHP - 6);
        player.currentHP = state.playerHP;
        state.actionPoints += 2;
        TakePile(3);
        DiscardPlayedCard(idx);
        break;

    default:
        break;
    }
}

void BattleLogic::waitPlayerInput(int idx, Enemy &enemy)
{
    if (idx < 0 || idx >= static_cast<int>(state.handCards.size()))
        return;

    const PileType type = state.handCards[idx].name;

    switch (type)
    {
    case PileType::Strike:
        if (!SpendActionPoints(1))
            return;
        DealDamage(enemy, 6);
        DiscardPlayedCard(idx);
        if(enemy.thornsdata>0) sufferThorns(enemy.thornsdata);
        break;

    case PileType::Rage:
        if (!SpendActionPoints(1))
            return;
        DealDamage(enemy, 8 + state.rampageBonus);
        state.rampageBonus += 5;
        DiscardPlayedCard(idx);
        if(enemy.thornsdata>0) sufferThorns(enemy.thornsdata);
        break;

    case PileType::Heavy_strike:
        if (!SpendActionPoints(2))
            return;
        DealDamage(enemy, 8);
        enemy.buff_debuff_vec.Add({2, BuffDebuffType::easy_to_attack});
        DiscardPlayedCard(idx);
        if(enemy.thornsdata>0) sufferThorns(enemy.thornsdata);
        break;

    case PileType::Anger:
        DealDamage(enemy, 6);
        state.discardPile.push_back(state.handCards[idx]);
        DiscardPlayedCard(idx);
        if(enemy.thornsdata>0) sufferThorns(enemy.thornsdata);
        break;

    case PileType::Continuous_punches:
        if (!SpendActionPoints(1))
            return;
        DealDamage(enemy, 2, 4);
        DiscardPlayedCard(idx);
        if(enemy.thornsdata>0) sufferThorns(enemy.thornsdata);
        break;

    case PileType::Observe_weaknesses:
        if (!SpendActionPoints(1))
            return;
        if (EnemyIntendsAttack(enemy))
            state.strength += 3;
        DiscardPlayedCard(idx);
        break;

    default:
        break;
    }
}

bool BattleLogic::BattleFinished(std::vector<Enemy *> enemies)
{
    if (state.playerHP <= 0)
    {
        events.push_back({EventType::EndBattle, 1});
        return true;
    }

    for (Enemy *enemy : enemies)
    {
        if (enemy && enemy->cur_health > 0)
            return false;
    }

    for (Enemy *enemy : enemies)
    {
        if (enemy)
            enemy->dead = true;
    }

    events.push_back({EventType::EndBattle, 0});
    return true;
}

void BattleLogic::StartBattle(const std::vector<Enemy> &initialEnemies,
                              const std::vector<Card> &cards,
                              Player &player)
{
    textPrompt->Show("kkkk", PromptStyle::Center);
    state = BattleState{};
    state.playerHP = player.GetCurrentHP();
    state.maxHP = player.GetMaxHP();
    state.cardSum = static_cast<int>(cards.size());
    state.enemies = initialEnemies;
    state.maxActionPoints = 3;
    state.actionPoints = state.maxActionPoints;
    state.dealPile = cards;
    state.isPlayerTurn = true;

    events.push_back({EventType::PlayerTurn});
}

void BattleLogic::EnemyTurn(Player &player, Enemy &enemy)
{
    if (enemy.allPlans.empty())
    {
        state.isPlayerTurn = true;
        events.push_back({EventType::PlayerTurn});
        return;
    }

    enemy.defend_num = 0;
    const int planIndex = std::max(0, state.TurnCount - 1) % static_cast<int>(enemy.allPlans.size());
    const Plan &plan = enemy.allPlans[planIndex];

    switch (plan.plantype)
    {
    case PlanType::attack:
    {
        int damage = std::max(0, plan.data);
        DealDamage(damage,enemy);
        if(state.thornsdata > 0) sufferThorns(enemy, state.thornsdata);
        player.currentHP = state.playerHP;
        break;
    }
    case PlanType::defend:
        enemy.defend_num += plan.data;
        break;
    case PlanType::easy_to_attack:
        state.buff_debuff_vec.Add({plan.data,BuffDebuffType::easy_to_attack});
        break;
    case PlanType::power_up:
        enemy.strength += plan.data;
        break;
    case PlanType::thorns:
        enemy.thornsdata += plan.data;
        break;
    case PlanType::vulnerable:
        state.buff_debuff_vec.Add({plan.data,BuffDebuffType::vulnerable});
        break;
    default:
        break;
    }

    enemy.buff_debuff_vec.update();
    state.isPlayerTurn = true;
    events.push_back({EventType::PlayerTurn});
}

int getRandomInt(int min, int max)
{
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

std::vector<PileType> BattleLogic::getHandCardsPile()
{
    std::vector<PileType> result;
    for (const auto &card : state.handCards)
        result.push_back(card.name);
    return result;
}

std::vector<PileType> BattleLogic::getdisCardPile()
{
    std::vector<PileType> result;
    for (const auto &card : state.discardPile)
        result.push_back(card.name);
    return result;
}

std::vector<PileType> BattleLogic::getdealCardsPile()
{
    std::vector<PileType> result;
    for (const auto &card : state.dealPile)
        result.push_back(card.name);
    return result;
}

bool BattleLogic::HasStatus(const std::vector<BDinfo> buff_debuffs, BuffDebuffType type)
{
    for (const auto &status : buff_debuffs)
    {
        if (status.type == type)
            return true;
    }
    return false;
}

void BattleLogic::sufferThorns(Enemy &enemy,int damage){
    enemy.cur_health -= damage;
}

void BattleLogic::sufferThorns(int damage){
    state.playerHP -= damage;
}