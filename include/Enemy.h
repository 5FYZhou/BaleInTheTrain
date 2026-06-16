#pragma once
#include "Constants.h"
#include <string>

// 敌人类型对应{敌人位置，掉落物}
inline const std::unordered_map<EnemyType, std::pair<sf::Vector2f, std::vector<ItemType>>> enemyBound = {
    { EnemyType::Train_attendant, {{ 213, 377 },{ItemType::Defend, ItemType::Key}} },
    { EnemyType::LightMonster, {{ 184, 376 },{}} },
    { EnemyType::TicketMonster, {{ 176, 294 },{}} }
};

enum class PlanType{
    attack,
    defend,
    buff,
    debuff,
};
struct Plan{
    int num_of_att_ot_def;
    PlanType plantype;
};
class Enemy {
    public:
        EnemyType id; // 敌人ID
        std::string name; // 敌人名称
        sf::Vector2f position; // 敌人位置（用于渲染）
        int frameIndex = 0; // 当前动画帧索引
        int frameCount = 2;
        float frameTimer = 0.f;
        float frameSpeed = 0.5;        // Frames per second for animation
        sf::FloatRect bound; // 碰撞体

        int sum_health; // 总生命值
        int cur_health; // 当前生命值
        int defend_num = 0;
        bool dead; // 是否死亡
        std::vector<Plan> allPlans; // 回合计划（0-攻击，1-防御，2-增益，3-减益）
        std::vector<StatusType> StatusEffect; // 状态效果（如中毒、虚弱等）
        std::vector<ItemType> droppedItems; // 被击败后掉落的物品

    public:
        Enemy(EnemyType id, sf::Vector2f p) : id(id), position(p), bound({p, enemyBound.at(id).first}), droppedItems({enemyBound.at(id).second}){}
        Enemy(EnemyType id, const std::string& name, int sum_health, std::vector<Plan> plans)
            : id(id), name(name), sum_health(sum_health), cur_health(sum_health), allPlans(plans) {}

        void Update(float dt){
            frameTimer += dt;
            if(frameTimer >= frameSpeed){
                frameIndex = (frameIndex + 1) % frameCount;
                frameTimer = 0.f;
            }
        }
};


inline std::vector<Plan> MakePlans(std::initializer_list<Plan> list) {
    return std::vector<Plan>(list);
}
const std::vector<Enemy> g_prefabEnemies = {
    // 示例1：列车员
    Enemy(
        EnemyType::Train_attendant,
        "列车员",
        40,
        MakePlans({
            {6, PlanType::attack},   // 攻击
            {4, PlanType::defend},   // 防御
            {8, PlanType::attack},  
            {4, PlanType::defend},
            {10, PlanType::attack},
            {4, PlanType::defend},
            {12, PlanType::attack},
            {4, PlanType::defend},
            {14, PlanType::attack},
            {4, PlanType::defend},
        })
    ),
    
    // 示例2：光之怪物
    Enemy(
        EnemyType::LightMonster,
        "光之怪物",
        80,
        MakePlans({
            {2, PlanType::attack},   // 连续攻击
            {0, PlanType::debuff}    // 减益
        })
    ),
    
    // 示例3：票务怪物
    Enemy(
        EnemyType::TicketMonster,
        "票务怪物",
        120,
        MakePlans({
            {1, PlanType::defend},
            {1, PlanType::attack},
            {1, PlanType::buff}
        })
    ),
    
    // === 后续追加新敌人时，直接在下面加 ===
    // Enemy(EnemyType::NewType, "新敌人", 150, MakePlans({...})),
};