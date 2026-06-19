#include "BattleLogic.h"

void BattleLogic::BattleLogicManager(const std::vector<Enemy> &Enemies, std::vector<Card> &Cards, Player &player)
{
    StartBattle(Enemies, Cards, player);
    std::vector<EventType> Btevents;

    // 更新
    // 渲染

    // 玩家选择战利品后进行卡组，药水，钥匙等更新
    // updatecards();
    // updatepotion;
    //.....
}

void BattleLogic::PilePre()
{
    state.TurnCount += 1;
    // 检测抽牌堆是否满足抽牌数，否则将弃牌堆中的牌全部加入抽牌堆
    if (state.dealNums > state.dealPile.size())
    {
        for (int i = 0; i < state.discardPile.size(); ++i)
        {
            state.dealPile.push_back(state.discardPile[i]);
            state.discardPile.erase(state.discardPile.begin() + i);
        }
    }
    // 从抽牌堆中抽牌,恢复行动点
    for (int i = 0; i < state.dealNums; ++i)
    {
        state.actionPoints = 3;
        int rd = getRandomInt(0, state.dealPile.size() - 1);
        state.handCards.push_back(state.dealPile[rd]);
        state.dealPile.erase(state.dealPile.begin() + rd);
    }
}

void BattleLogic::turnsOver()
{
    state.isPlayerTurn = false;
    // 手牌丢到弃牌堆中
    state.discardPile.insert(state.discardPile.end(), state.handCards.begin(), state.handCards.end());
    state.handCards.clear();
}

void BattleLogic::PlayerStatusSettlement() // 玩家状态结算
{
    state.defend_num = 0;
}

void BattleLogic::waitPlayerInput(int idx)
{
    PileType ty = state.handCards[idx].name;
    // 卡牌效果
    switch (ty)
    {
    case PileType::Defend:
        if (state.actionPoints < 1)
        {
            std::cout << "行动点不足" << std::endl;
            break;
        }
        state.defend_num += 5;
        state.actionPoints--;
        state.discardPile.push_back(state.handCards[idx]);
        state.handCards.erase(state.handCards.begin() + idx);

        break;

    default:
        break;
    }

    // 删除卡牌
}
void BattleLogic::waitPlayerInput(int idx, Enemy &enemy)
{
    PileType ty = state.handCards[idx].name;

    switch (ty)
    {
    case PileType::Strike:
        if (state.actionPoints < 1)
        {
            std::cout << "行动点不足" << std::endl;
            break;
        }
        enemy.cur_health -= 6;
        if (enemy.cur_health < 0)
            enemy.cur_health = 0;
        state.actionPoints--;
        state.discardPile.push_back(state.handCards[idx]);
        state.handCards.erase(state.handCards.begin() + idx);
        break;

    default:
        break;
    }

    // 删除卡牌
}

bool BattleLogic::BattleFinished(std::vector<Enemy> eys)
{
    // 检测玩家和敌人血量
    if (state.playerHP <= 0)
    {
        events.push_back({EventType::EndBattle, 1});
        return true;
    }
    for (auto &it : eys)
    {
        if (it.cur_health > 0)
        {
            std::cout << "no die" << it.cur_health<< std::endl;
            return false;
        }
        else{
            it.dead = true;
        }
    }
    std::cout << "die" << std::endl;
    
    events.push_back({EventType::EndBattle, 0});

    return true;
}

void BattleLogic::StartBattle(const std::vector<Enemy> &initialEnemies,
                              const std::vector<Card> &Cards, Player &player)
{
    // 初始化玩家血量
    state.playerHP = player.GetCurrentHP();
    state.maxHP = player.GetMaxHP();
    /*获得药水数据*/
    /*获得遗物数据*/

    // 卡牌总数
    state.cardSum = Cards.size();

    // 获得敌人数据（退出战斗时应销毁
    for (auto it : initialEnemies)
    {
        state.enemies.push_back(it);
    }
    // 初始化行动点
    state.actionPoints = 3;
    state.maxActionPoints = 3;

    // 获得卡牌数据
    for (auto it : Cards)
    {
        state.dealPile.push_back(it);
    }

    state.cardIsUsed = false;

    // 初始化回合
    state.TurnCount = 0;
    state.isPlayerTurn = true;

    events.push_back({EventType::PlayerTurn});
}

void BattleLogic::EnemyTurn(Player &player) // 敌人行动，然后切回玩家
{
    // 敌人行动逻辑
    for (auto &enemy : state.enemies)
    {
        PlanType ty = enemy.allPlans[state.TurnCount - 1].plantype;
        int data = enemy.allPlans[state.TurnCount - 1].num_of_att_ot_def;
        switch (ty)
        {
        case PlanType::attack:
            if (data > state.defend_num)
            {
                state.playerHP -= (data - state.defend_num);
                state.defend_num = 0;
            }
            else
            {
                state.defend_num -= data;
            }
            if (state.playerHP < 0)
                state.playerHP = 0;
            player.currentHP = state.playerHP;
            break;

        case PlanType::defend:
            enemy.defend_num = data;
        default:
            break;
        }
    }

    // 切回玩家回合A
    state.isPlayerTurn = true;
    events.push_back({EventType::PlayerTurn});
}

int getRandomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

std::vector<PileType> BattleLogic::getHandCardsPile()
{
    std::vector<PileType> v;
    for (auto it : state.handCards)
    {
        v.push_back(it.name);
    }
    return v;
}
std::vector<PileType> BattleLogic::getdisCardPile()
{
    std::vector<PileType> v;
    for (auto it : state.discardPile)
    {
        v.push_back(it.name);
    }
    return v;
}
std::vector<PileType> BattleLogic::getdealCardsPile()
{
    std::vector<PileType> v;
    for (auto it : state.dealPile)
    {
        v.push_back(it.name);
    }
    return v;
}