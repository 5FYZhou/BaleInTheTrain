#include "BattleLogic.h"

void BattleLogic::BattleLogicManager(const std::vector<Enemy> &Enemies, std::vector<Card> &Cards, Player &player)
{
    StartBattle(Enemies, Cards, player);
    while (!BattleFinished(player))
    {
        // 交互

        // 更新
        BattleUpdate();
        // 渲染
    }
    // 玩家选择战利品后进行卡组，药水，钥匙等更新
    // updatecards();
    // updatepotion;
    //.....
}

void BattleLogic::BattleUpdate()
{
    std::vector<EventType> Btevents;
    while (true)
    {
        // 检测玩家和敌人血量
        if (state.playerHP == 0)
        {
            Btevents.push_back(EventType::EndBattle);
            break;
        }
        bool allEnemiesIsDie = true;
        for (auto it : state.enemies)
        {
            if (it.cur_health > 0)
                allEnemiesIsDie = false;
        }
        if (allEnemiesIsDie == true)
        {
            Btevents.push_back(EventType::EndBattle);
            break;
        }

        //玩家回合部分逻辑
        if (state.isPlayerTurn)
        {
            PilePre();//抽牌
            PlayerStatusSettlement();//玩家状态结算
        }
        else{//敌人回合
            EnemyTurn();
        }
    }
    
}

void BattleLogic::PilePre()
{
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
        int rd = getRandomInt(0, state.cardSum);
        state.handCards.push_back(state.dealPile[rd]);
        state.dealPile.erase(state.dealPile.begin() + rd);
    }
}

void BattleLogic::turnsOver()
{
    state.isPlayerTurn = false;
    // 手牌丢到弃牌堆中
    for (int i = 0; i < state.handCards.size(); ++i)
    {
        state.discardPile.push_back(state.handCards[i]);
        state.discardPileCount++;
        state.handCards.erase(state.handCards.begin() + i);
    }
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
        state.defend_num += 5;
        break;

    default:
        break;
    }

    // 删除卡牌
    state.handCards.erase(state.handCards.begin() + idx);
}
void BattleLogic::waitPlayerInput(int idx, Enemy &enemy)
{
    PileType ty = state.handCards[idx].name;

    switch (ty)
    {
    case PileType::Strike:
        enemy.cur_health -= 6;
        break;

    default:
        break;
    }

    // 删除卡牌
    state.handCards.erase(state.handCards.begin() + idx);
}

void BattleLogic::HandleInput(const sf::Vector2f &mousePos)
{
    for (auto &enemy : state.enemies)
    {
        if (enemy.bound.contains(mousePos))
        {
        }
    }
}

bool BattleLogic::BattleFinished(Player &player)
{
    if (state.isWin)
    {
        // 显示选择战利品的界面

        player.SetHP(state.playerHP, state.maxHP);
        return true;
    }
    if (state.isLose)
    {
        // 显示战斗失败的界面

        player.SetHP(state.playerHP, state.maxHP);
        return true;
    }
    return false;
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
    state.dealPileCount = Cards.size();
    state.discardPileCount = 0;
    state.cardIsUsed = false;

    // 初始化回合
    state.TurnCount = 1;
    state.isPlayerTurn = true;
}

void BattleLogic::EnemyTurn() // 敌人行动，然后切回玩家
{
    // 敌人行动逻辑
    // for (auto &enemy : state.enemies)
    // {
    //     if (enemy.IsAlive())
    //     {
    //         enemy.PerformAction();
    //     }
    // }

    // 切回玩家回合A
    state.isPlayerTurn = true;
}

int getRandomInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
