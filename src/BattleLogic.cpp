#include "BattleLogic.h"

void BattleLogic::BattleLogicManager(const std::vector<Enemy> &Enemies, std::vector<Card> &Cards, Player &player)
{
    StartBattle(Enemies, Cards, player);
    while (!BattleFinished(player))
    {
        // 交互
        const MouseState mouseState = input.GetMouseState(window);
            const sf::Vector2f mousePos{
                static_cast<float>(mouseState.position.x),
                static_cast<float>(mouseState.position.y)
            };
        HandleInput(const sf::Vector2f& mousePos);
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
    if (state.isPlayerTurn)
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
        // 一些buff，debuff结算

        // 玩家操作
        while (state.isPlayerTurn)
        {
            // 如果鼠标悬停在牌上，卡牌会向上浮起，露出完整的效果描述

            if (如果左键点击卡牌)
            {
                state.cardIsUsed = true;
                while (检测鼠标)
                {
                    if (右键)
                    {
                        state.cardIsUsed = false;
                    }
                    if (左键点击目标)
                    {
                        for (auto enemy : state.enemies)
                        {
                            if (enemy.bound.contains(mousePos))
                            {
                            }
                        }
                        if (是玩家)
                        {
                        }
                    }
                }
            }

            // 如果鼠标悬停在药水上，显示药水效果

            // if(如果左键点击药水)
            //{
            //
            // }

            if (点击结束按钮)
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
        }
    }
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

void BattleLogic::EndPlayerTurn() // 敌人行动，然后切回玩家
{
    // 敌人行动逻辑
    for (auto &enemy : state.enemies)
    {
        if (enemy.IsAlive())
        {
            enemy.PerformAction();
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
