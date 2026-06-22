#pragma once
#include "Constants.h"
#include "BuffAndDebuff.h"
#include <string>

// 敌人类型对应{敌人贴图大小，掉落物，绘制血量偏移}
inline const std::unordered_map<EnemyType, std::tuple<sf::Vector2f, std::vector<ItemType>, int>> enemyBound = {
    {EnemyType::Train_attendant, {{213, 377}, {ItemType::Defend, ItemType::Key}, 10}},
    {EnemyType::LightMonster, {{184, 376}, {}, 0}},
    {EnemyType::TicketMonster, {{176, 294}, {}, 0}},
    {EnemyType::TyreMosnter, {{223, 200}, {}, 0}},
    {EnemyType::Past_YOU, {{360, 367}, {}, 10}},
};

struct Plan
{
    int data;
    PlanType plantype;
};
struct enemy_data
{
    EnemyType ty;
    std::string name;
    int maxHP;
    std::vector<Plan> plans;
};
class Enemy
{
public:
    EnemyType ty;          // 敌人ID
    std::string name;      // 敌人名称
    sf::Vector2f position; // 敌人位置（用于渲染）
    sf::Vector2f initPos; // 敌人原来位置（魂用）
    int frameIndex = 0;    // 当前动画帧索引
    int frameCount = 2;
    float frameTimer = 0.f;
    float frameSpeed = 0.5;          // Frames per second for animation
    sf::Vector2f scale = {1.0, 1.0}; // 绘制和碰撞体缩放
    sf::FloatRect bound;             // 碰撞体
    float HPDrawOffset;              // 绘制血量时相对敌人位置的y轴偏移

    int sum_health; // 总生命值
    int cur_health; // 当前生命值
    bool dead;      // 是否死亡

    std::vector<Plan> allPlans;         // 回合计划（0-攻击，1-防御，2-增益，3-减益）

    
    int strength = 0;   //攻击时增加对应的力量值
    int thornsdata = 0;  //被攻击时造成的反伤
    int defend_num = 0;
    Buff_Debuff_Vec buff_debuff_vec;    // 状态效果（如中毒、虚弱等）
    std::vector<ItemType> droppedItems; // 被击败后掉落的物品

public:
    Enemy(EnemyType id, sf::Vector2f p, sf::Vector2f s = {1.0f, 1.0f})
        : ty(id), position(p), scale(s),
          droppedItems(std::get<1>(enemyBound.at(id))),
          HPDrawOffset(std::get<2>(enemyBound.at(id)))
    {
        initPos = position;
        auto size = std::get<0>(enemyBound.at(id));

        bound = sf::FloatRect(
            p,
            {scale.x * size.x,
             scale.y * size.y});
    }

    void Update(float dt)
    {
        frameTimer += dt;
        if (frameTimer >= frameSpeed)
        {
            frameIndex = (frameIndex + 1) % frameCount;
            frameTimer = 0.f;
        }
    }
    void SetPosX(float x)
    {
        position.x = x;
        bound.position.x = x;
    }
};

inline std::vector<Plan> MakePlans(std::initializer_list<Plan> list)
{
    return std::vector<Plan>(list);
}
// 使用 unordered_map 存储预制敌人数据，键为 EnemyType
const std::unordered_map<EnemyType, enemy_data> g_prefabEnemies = {
    // 示例1：列车员
    {EnemyType::Train_attendant,
     enemy_data{
         EnemyType::Train_attendant,
         "列车员",
         40,
         std::vector<Plan>({
             {6, PlanType::attack},
             {4, PlanType::defend},
             {8, PlanType::attack},
             {4, PlanType::defend},
             {10, PlanType::attack},
             {4, PlanType::defend},
             {12, PlanType::attack},
             {4, PlanType::defend},
             {14, PlanType::attack},
             {4, PlanType::defend},
             {16, PlanType::attack},
             {4, PlanType::defend},
             {18, PlanType::attack},
             {4, PlanType::defend},
             {20, PlanType::attack},
             {4, PlanType::defend},
             {22, PlanType::attack},
             {4, PlanType::defend},
             {24, PlanType::attack},
             {4, PlanType::defend},
             {26, PlanType::attack},
             {4, PlanType::defend},
             {28, PlanType::attack},
             {4, PlanType::defend},
             {30, PlanType::attack},
             {4, PlanType::defend},
             {32, PlanType::attack},
             {4, PlanType::defend},
             {34, PlanType::attack},
             {4, PlanType::defend},
             {36, PlanType::attack},
             {4, PlanType::defend},
             {38, PlanType::attack},
             {4, PlanType::defend},
             {40, PlanType::attack},
             {4, PlanType::defend},
             {42, PlanType::attack},
             {4, PlanType::defend},
             {44, PlanType::attack},
             {4, PlanType::defend},
             {46, PlanType::attack},
             {4, PlanType::defend},
             {48, PlanType::attack},
             {4, PlanType::defend},
             {50, PlanType::attack},
             {4, PlanType::defend},
             {52, PlanType::attack},
             {4, PlanType::defend},
             {54, PlanType::attack},
             {4, PlanType::defend},
             {56, PlanType::attack},
             {4, PlanType::defend},
             {58, PlanType::attack},
             {4, PlanType::defend},
             {60, PlanType::attack},
             {4, PlanType::defend},
             {62, PlanType::attack},
             {4, PlanType::defend},
             {64, PlanType::attack},
             {4, PlanType::defend},
             {66, PlanType::attack},
             {4, PlanType::defend},
             {68, PlanType::attack},
             {4, PlanType::defend},
             {70, PlanType::attack},
             {4, PlanType::defend},
             {72, PlanType::attack},
             {4, PlanType::defend},
             {74, PlanType::attack},
             {4, PlanType::defend},
             {76, PlanType::attack},
             {4, PlanType::defend},
             {78, PlanType::attack},
             {4, PlanType::defend},
             {80, PlanType::attack},
             {4, PlanType::defend},
             {82, PlanType::attack},
             {4, PlanType::defend},
             {84, PlanType::attack},
             {4, PlanType::defend},
         })}},

    // 示例2：车灯
    {EnemyType::LightMonster,
     enemy_data{
         EnemyType::LightMonster,
         "车灯",
         50,
         std::vector<Plan>({
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},

             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7,PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},

             {2, PlanType::easy_to_attack},
             {8, PlanType::attack},
             {6, PlanType::attack},
             {2, PlanType::easy_to_attack},
             {10, PlanType::attack},
             {7, PlanType::attack},
             {2, PlanType::vulnerable},
             {12, PlanType::attack},
             {15, PlanType::defend},
         })}},

    // 示例3：车票怪
    {EnemyType::TicketMonster,
     enemy_data{
         EnemyType::TicketMonster,
         "车票怪",
         60,
         std::vector<Plan>({
             {8, PlanType::attack},
             {2, PlanType::power_up},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::power_up},
             {10, PlanType::attack},
             {8,PlanType::defend},
             {8, PlanType::attack},
             {2, PlanType::power_up},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::power_up},
             {10, PlanType::attack},
             {8,PlanType::defend},
             {8, PlanType::attack},
             {2, PlanType::power_up},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::power_up},
             {10, PlanType::attack},
             {8,PlanType::defend},
             {8, PlanType::attack},
             {2, PlanType::power_up},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::power_up},
             {10, PlanType::attack},
             {8,PlanType::defend},
             {8, PlanType::attack},
             {2, PlanType::power_up},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::power_up},
             {10, PlanType::attack},
             {8,PlanType::defend},

             {8, PlanType::attack},
             {2, PlanType::power_up},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::power_up},
             {10, PlanType::attack},
             {8,PlanType::defend},
         })}},
    // 车轮怪
    {EnemyType::TicketMonster,
     enemy_data{
         EnemyType::TicketMonster,
         "车轮怪",
         50,
         std::vector<Plan>({
             {2, PlanType::thorns},
             {5, PlanType::attack},
             {2, PlanType::thorns},
             {6, PlanType::attack},
             {3, PlanType::thorns},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::thorns},
             {7, PlanType::attack},
             {10,PlanType::defend},

             {2, PlanType::thorns},
             {5, PlanType::attack},
             {2, PlanType::thorns},
             {6, PlanType::attack},
             {3, PlanType::thorns},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::thorns},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {2, PlanType::thorns},
             {5, PlanType::attack},
             {2, PlanType::thorns},
             {6, PlanType::attack},
             {3, PlanType::thorns},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::thorns},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {2, PlanType::thorns},
             {5, PlanType::attack},
             {2, PlanType::thorns},
             {6, PlanType::attack},
             {3, PlanType::thorns},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::thorns},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {2, PlanType::thorns},
             {5, PlanType::attack},
             {2, PlanType::thorns},
             {6, PlanType::attack},
             {3, PlanType::thorns},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::thorns},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {2, PlanType::thorns},
             {5, PlanType::attack},
             {2, PlanType::thorns},
             {6, PlanType::attack},
             {3, PlanType::thorns},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::thorns},
             {7, PlanType::attack},
             {10,PlanType::defend},
         })}},
    // 过去死亡的你
    {EnemyType::Past_YOU,
     enemy_data{
         EnemyType::Past_YOU,
         "？？？（█澶辫触鑰�）",
         100,
         std::vector<Plan>({
             {4, PlanType::thorns},
             {10, PlanType::attack},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {2, PlanType::vulnerable},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::easy_to_attack},
             {7, PlanType::attack},
             {10,PlanType::defend},


             {4, PlanType::thorns},
             {10, PlanType::attack},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {2, PlanType::vulnerable},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::easy_to_attack},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {4, PlanType::thorns},
             {10, PlanType::attack},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {2, PlanType::vulnerable},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::easy_to_attack},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {4, PlanType::thorns},
             {10, PlanType::attack},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {2, PlanType::vulnerable},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::easy_to_attack},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {4, PlanType::thorns},
             {10, PlanType::attack},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {2, PlanType::vulnerable},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::easy_to_attack},
             {7, PlanType::attack},
             {10,PlanType::defend},
             {4, PlanType::thorns},
             {10, PlanType::attack},
             {8, PlanType::attack},
             {3, PlanType::power_up},
             {2, PlanType::vulnerable},
             {8, PlanType::attack},
             {10, PlanType::attack},
             {3, PlanType::easy_to_attack},
             {7, PlanType::attack},
             {10,PlanType::defend},
         })}},

};