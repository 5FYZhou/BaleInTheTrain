#include "BattleLogic.h"

void BattleLogic::BattleLogicManager(const std::vector<Enemy> &Enemies, std::vector<Card> &Cards, Player &player)
{
    StartBattle(Enemies, Cards, player);
    std::vector<EventType> Btevents;

    // 检测玩家和敌人血量
    if (state.playerHP == 0)
    {
        Btevents.push_back(EventType::EndBattle);
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
    }

    // 更新
    BattleUpdate();
    // 渲染

    // 玩家选择战利品后进行卡组，药水，钥匙等更新
    // updatecards();
    // updatepotion;
    //.....
}

void BattleLogic::BattleUpdate()
{

    // 玩家回合部分逻辑
    if (state.isPlayerTurn)
    {
    }
    else
    { // 敌人回合
    }
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
    std::cout << "Blgic:"<<state.handCards.size()<<" "<<
               state.dealPile.size()<<" "<<
                state.discardPile.size()<<" "<<std::endl;
    std::cout << "state.dealNums:" << state.dealNums << std::endl;
    // 从抽牌堆中抽牌,恢复行动点
    for (int i = 0; i < state.dealNums; ++i)
    {
        state.actionPoints = 3;
        int rd = getRandomInt(0, state.dealPile.size()-1);
        state.handCards.push_back(state.dealPile[rd]);
        state.dealPile.erase(state.dealPile.begin() + rd);
    }
    std::cout << "BattleLogic::PilePre end" << std::endl;
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
        enemy.cur_health -= 6;
        state.handCards.erase(state.handCards.begin() + idx);
        break;

    default:
        break;
    }

    // 删除卡牌
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
    state.TurnCount = 0;
    state.isPlayerTurn = true;

    events.push_back({EventType::PlayerTurn});
}

void BattleLogic::EnemyTurn() // 敌人行动，然后切回玩家
{
    // 敌人行动逻辑
    for (auto &enemy : state.enemies)
    {
        std::cout<<"BattleLogic::EnemyTurn"<<enemy.allPlans.size()<<std::endl;
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
            break;

        case PlanType::defend:
            enemy.defend_num = data;
        default:
            break;
        }
    }

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

std::vector<PileType> BattleLogic::getCardsPile()
{
    std::vector<PileType> v;
    for(auto it: state.handCards){
        v.push_back(it.name);
    }
    return v;
}