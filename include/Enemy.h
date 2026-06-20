#pragma once
#include "Constants.h"
#include "BuffAndDebuff.h"
#include <string>

// 敌人类型对应{敌人位置，掉落物，绘制血量偏移}
inline const std::unordered_map<EnemyType, std::tuple<sf::Vector2f, std::vector<ItemType>, int>> enemyBound = {
    {EnemyType::Train_attendant, {{213, 377}, {ItemType::Defend, ItemType::Key}, -30}},
    {EnemyType::LightMonster, {{184, 376}, {}, -30}},
    {EnemyType::TicketMonster, {{176, 294}, {}, -30}}};

struct Plan
{
    int num_of_att_ot_def;
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
    int frameIndex = 0;    // 当前动画帧索引
    int frameCount = 2;
    float frameTimer = 0.f;
    float frameSpeed = 0.5; // Frames per second for animation
    sf::FloatRect bound;    // 碰撞体
    float HPDrawOffset;     // 绘制血量时相对敌人位置的y轴偏移

    int sum_health; // 总生命值
    int cur_health; // 当前生命值
    int defend_num = 0;
    bool dead;                          // 是否死亡
    std::vector<Plan> allPlans;         // 回合计划（0-攻击，1-防御，2-增益，3-减益）
    int strength = 0;
    Buff_Debuff_Vec buff_debuff_vec;       // 状态效果（如中毒、虚弱等）
    std::vector<ItemType> droppedItems; // 被击败后掉落的物品

public:
    Enemy(EnemyType id, sf::Vector2f p) : ty(id), position(p),
                                          bound({p, std::get<0>(enemyBound.at(id))}),
                                          droppedItems(std::get<1>(enemyBound.at(id))),
                                          HPDrawOffset(std::get<2>(enemyBound.at(id))) {}
    Enemy(enemy_data ed)
        : ty(ed.ty), name(ed.name), sum_health(ed.maxHP), cur_health(ed.maxHP), allPlans(ed.plans) {}

    void Update(float dt)
    {
        frameTimer += dt;
        if (frameTimer >= frameSpeed)
        {
            frameIndex = (frameIndex + 1) % frameCount;
            frameTimer = 0.f;
        }
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
             {5, PlanType::attack},
             {3, PlanType::defend},
             {7, PlanType::attack},
             {3, PlanType::defend},
         })}},

    // 示例3：车票怪
    {EnemyType::TicketMonster,
     enemy_data{
         EnemyType::TicketMonster,
         "车票怪",
         30,
         std::vector<Plan>({
             {4, PlanType::attack},
             {5, PlanType::defend},
             {6, PlanType::attack},
             {5, PlanType::defend},
         })}},
};