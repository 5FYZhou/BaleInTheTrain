#include "BattleLogic.h"

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

bool BattleLogic::EnemyIntendsAttack() const
{
    if (enemy->allPlans.empty())
        return false;

    const int planIndex = std::max(0, state.TurnCount - 1) % static_cast<int>(enemy->allPlans.size());
    return enemy->allPlans[planIndex].plantype == PlanType::attack;
}

void BattleLogic::DealDirectDamage(Enemy* enemy, int damage)
{
    damage = std::max(0, damage);
    const int blocked = std::min(enemy->defend_num, damage);
    enemy->defend_num -= blocked;
    damage -= blocked;
    //enemy->cur_health = std::max(0, enemy->cur_health - damage);
    UpdateHP(enemy, -damage);
}

void BattleLogic::DealDirectDamage(int damage)
{
    damage = std::max(0, damage);
    const int blocked = std::min(state.defend_num, damage);
    state.defend_num -= blocked;
    damage -= blocked;
    state.final_damage = damage;
    UpdateHP(player, -damage);
}

void BattleLogic::DealDamage(Enemy* enemy, int baseDamage, int hitCount, bool applyStrength)
{
    for (int hit = 0; hit < hitCount && enemy->cur_health > 0; ++hit)
    {
        int damage = baseDamage;
        if (applyStrength)
            damage += state.strength + state.temporaryStrength;

        if (HasStatus(enemy->buff_debuff_vec.buff_debuffs, BuffDebuffType::easy_to_attack))
            damage = static_cast<int>(damage * 1.5);

        if (HasStatus(state.buff_debuff_vec.buff_debuffs, BuffDebuffType::vulnerable))
            damage = static_cast<int>(damage * 0.75);

        DealDirectDamage(enemy, damage);
    }
}

void BattleLogic::DealDamage(int baseDamage, Enemy* enemy, bool applyStrength)
{
    int damage = baseDamage;
    if (applyStrength)
        damage += enemy->strength;

    if (HasStatus(state.buff_debuff_vec.buff_debuffs, BuffDebuffType::easy_to_attack))
        damage = static_cast<int>(damage * 1.5);

    if (HasStatus(enemy->buff_debuff_vec.buff_debuffs, BuffDebuffType::vulnerable))
        damage = static_cast<int>(damage * 0.75);

    DealDirectDamage(damage);
}

void BattleLogic::GainBlock(int amount, Enemy *enemy)
{
    if (amount <= 0)
        return;

    state.defend_num += amount;

    if (enemy && enemy->cur_health > 0 && state.juggernautDamage > 0)
        DealDirectDamage(enemy, state.juggernautDamage);
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

void BattleLogic::turnsOver()
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
    state.buff_debuff_vec.Add({-2, BuffDebuffType::power_up});
    if (!state.barricade)
        state.defend_num = 0;

    state.buff_debuff_vec.update();
}

void BattleLogic::ClickPlayer(int idx)
{
    std::cout << "BattleLogic: GlobalaudioMgr address = " << &GlobalaudioMgr << std::endl;
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
        state.buff_debuff_vec.Add({2, BuffDebuffType::power_up});
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

        GainBlock((exhaustedCount+1) * 5, enemy);
        DiscardPlayedCard(idx);
        break;
    }

    case PileType::Metallization:
        if (!SpendActionPoints(1))
            return;
        state.metallicize += 3;
        state.buff_debuff_vec.Add({-1, BuffDebuffType::metallization});
        DiscardPlayedCard(idx);
        break;

    case PileType::Unstoppable:
        if (!SpendActionPoints(2))
            return;
        state.juggernautDamage += 5;
        state.buff_debuff_vec.Add({-1, BuffDebuffType::unstoppable});
        DiscardPlayedCard(idx);

        break;

    case PileType::Rampart:
        if (!SpendActionPoints(3))
            return;
        state.barricade = true;
        state.buff_debuff_vec.Add({-1, BuffDebuffType::rampart});
        DiscardPlayedCard(idx);
        break;

    case PileType::Sacrifice:
        UpdateHP(player, -6);
        state.actionPoints += 2;
        TakePile(3);
        DiscardPlayedCard(idx);
        break;

    default:
        std::cout << "shit" << std::endl;
        GlobalaudioMgr.PlaySound(SoundEffect::ObjectError);
        break;
    }
}

void BattleLogic::ClickEnemy(int idx)
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
        if(enemy->thornsdata>0) sufferThorns(player, enemy->thornsdata);
        break;

    case PileType::Rage:
        if (!SpendActionPoints(1))
            return;
        DealDamage(enemy, 8 + state.rampageBonus);
        state.rampageBonus += 5;
        DiscardPlayedCard(idx);
        if(enemy->thornsdata>0) sufferThorns(player, enemy->thornsdata);
        break;

    case PileType::Heavy_strike:
        if (!SpendActionPoints(2))
            return;
        DealDamage(enemy, 8);
        enemy->buff_debuff_vec.Add({2, BuffDebuffType::easy_to_attack});
        DiscardPlayedCard(idx);
        if(enemy->thornsdata>0) sufferThorns(player, enemy->thornsdata);
        break;

    case PileType::Anger:
        DealDamage(enemy, 6);
        state.discardPile.push_back(state.handCards[idx]);
        DiscardPlayedCard(idx);
        if(enemy->thornsdata>0) sufferThorns(player, enemy->thornsdata);
        break;

    case PileType::Continuous_punches:
        if (!SpendActionPoints(1))
            return;
        DealDamage(enemy, 2, 4);
        DiscardPlayedCard(idx);
        if(enemy->thornsdata>0) sufferThorns(player, enemy->thornsdata);
        break;

    case PileType::Observe_weaknesses:
        if (!SpendActionPoints(1))
            return;
        if (EnemyIntendsAttack()){
            state.strength += 3;
            state.buff_debuff_vec.Add({3, BuffDebuffType::power_up});
        }        
        DiscardPlayedCard(idx);
        break;

    default:
        std::cout << "shit+" << std::endl;
        GlobalaudioMgr.PlaySound(SoundEffect::ObjectError);
        break;
    }
}

bool BattleLogic::BattleFinished()
{
    if (player && player->currentHP <= 0)
    {
        if(state.battleEnded) return true;
        std::cout<<"playerDead"<<std::endl;
        events.push_back({EventType::EndBattle, 1});
        return true;
    }

    if (enemy && enemy->cur_health <= 0)
    {
        if(state.battleEnded) return true;
        enemy->dead = true;
        std::cout<<"enemyDead"<<std::endl;
        float endi = enemy->ty == EnemyType::Past_YOU ? 2 : 0;
        events.push_back({EventType::EndBattle, endi});
        return true;
    }

    return false;
}

void BattleLogic::StartBattle(Enemy* e, Player* p)
{
    enemy = e;
    player = p;
    state = BattleState{};
    state.cardSum = player->cards.size();
    state.maxActionPoints = 3;
    state.actionPoints = state.maxActionPoints;
    state.dealPile = player->cards;
    state.isPlayerTurn = true;

    events.push_back({EventType::PlayerTurn});
}

void BattleLogic::EnemyTurn()
{
    if (enemy->allPlans.empty())
    {
        state.isPlayerTurn = true;
        events.push_back({EventType::PlayerTurn});
        std::cout << "敌人意图为空" << std::endl;
        return;
    }

    enemy->defend_num = 0;
    const int planIndex = std::max(0, state.TurnCount - 1) % static_cast<int>(enemy->allPlans.size());
    //std::cout << "planIndex:" << planIndex << std::endl;
    
    const Plan &plan = enemy->allPlans[planIndex];
    //std::cout << "diren:" << static_cast<int>(plan.plantype) << std::endl;

    switch (plan.plantype)
    {
    case PlanType::attack:
    {
        int damage = std::max(0, plan.data);
        DealDamage(damage,enemy);
        ShowEnemyDamage(*enemy);
        if(state.thornsdata > 0) sufferThorns(enemy, state.thornsdata);
        break;
    }
    case PlanType::defend:
        enemy->defend_num += plan.data;
        break;
    case PlanType::easy_to_attack:
        state.buff_debuff_vec.Add({plan.data,BuffDebuffType::easy_to_attack});
        break;
    case PlanType::power_up:
        enemy->strength += plan.data;
        enemy->buff_debuff_vec.Add({plan.data,BuffDebuffType::power_up});
        break;
    case PlanType::thorns:
        enemy->thornsdata += plan.data;
        enemy->buff_debuff_vec.Add({plan.data,BuffDebuffType::thorns});
        break;
    case PlanType::vulnerable:
        state.buff_debuff_vec.Add({plan.data,BuffDebuffType::vulnerable});
        break;
    default:
        break;
    }

    enemy->buff_debuff_vec.update();
    
    if(state.battleEnded) return;
    
    // 不立即设置 isPlayerTurn，而是启动延迟
    state.isPlayerTurn = false;
    isWaitingForPlayerTurn = true;
    playerTurnDelayClock.restart();
    std::cout << "Enemy turn finished, waiting 1s before PlayerTurn" << std::endl;
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

void BattleLogic::sufferThorns(Enemy* e,int damage){
    UpdateHP(e, -damage);
}

void BattleLogic::sufferThorns(Player* p, int damage){
    UpdateHP(p, -damage);
}

void BattleLogic::UpdateHP(Player* p, int add){
    if(add <= 0) p->currentHP = std::max(0, p->currentHP + add);
    else p->currentHP = std::min(p->currentHP + add, p->maxHP);
    BattleFinished();
}

void BattleLogic::UpdateHP(Enemy* p, int add){
    if(add <= 0) p->cur_health = std::max(0, p->cur_health + add);
    else p->cur_health = std::min(p->cur_health + add, p->sum_health);
    BattleFinished();
}


void BattleLogic::ShowTurnCounts(){
    if(state.battleEnded) return;
    std::string turnsText = "第" + std::to_string(state.TurnCount) + "回合";
    textPrompt->Show((turnsText), PromptStyle::Top);
}

void BattleLogic::ShowEnemyDamage(Enemy &enemy){
    if(state.battleEnded) return;
    std::string Enemy_damage = enemy.name + "对玩家造成了" + std::to_string(state.final_damage) +"点伤害!";
    textPrompt->Show((Enemy_damage), PromptStyle::Center);
}

void BattleLogic::Update()
{
    if (isWaitingForPlayerTurn && playerTurnDelayClock.getElapsedTime().asMilliseconds() >= PLAYER_TURN_DELAY_MS)
    {
        isWaitingForPlayerTurn = false;
        state.isPlayerTurn = true;
        events.push_back({EventType::PlayerTurn});
        std::cout << "PlayerTurn event pushed after delay" << std::endl;
    }
}