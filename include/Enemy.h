#pragma once
#include "Constants.h"
#include <string>

// 敌人类型对应{敌人位置，掉落物，绘制血量偏移}
inline const std::unordered_map<EnemyType, std::tuple<sf::Vector2f, std::vector<ItemType>, int>> enemyBound = {
    { EnemyType::Train_attendant, {{ 213, 377 },{ItemType::Defend, ItemType::Key}, -30} },
    { EnemyType::LightMonster, {{ 184, 376 },{}, -30} },
    { EnemyType::TicketMonster, {{ 176, 294 },{}, -30} }
};

enum class PlanType{
    attack,
    defend,
    buff,
    debuff,
};
struct Plan{
    int num_of_att_ot_def;
    PlanType plan;
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
        float HPDrawOffset; // 绘制血量时相对敌人位置的y轴偏移

        int sum_health = 100; // 总生命值
        int cur_health = 100; // 当前生命值
        bool dead; // 是否死亡
        std::vector<Plan> allPlans; // 下一回合计划（0-攻击，1-防御，2-增益，3-减益）
        std::vector<StatusType> StatusEffect; // 状态效果（如中毒、虚弱等）
        std::vector<ItemType> droppedItems; // 被击败后掉落的物品

    public:
        Enemy(EnemyType id, sf::Vector2f p) : id(id), position(p), 
            bound({p, std::get<0>(enemyBound.at(id))}), 
            droppedItems(std::get<1>(enemyBound.at(id))),
            HPDrawOffset(std::get<2>(enemyBound.at(id))){}
        Enemy(EnemyType id, const std::string& name, int sum_health, int num_of_turns, std::vector<Plan> plans)
            : id(id), name(name), sum_health(sum_health), cur_health(sum_health), allPlans(plans) {}

        void Update(float dt){
            frameTimer += dt;
            if(frameTimer >= frameSpeed){
                frameIndex = (frameIndex + 1) % frameCount;
                frameTimer = 0.f;
            }
        }
};