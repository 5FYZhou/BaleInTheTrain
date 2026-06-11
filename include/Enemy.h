#pragma once
#include "Constants.h"
#include <string>

inline const std::unordered_map<EnemyType, sf::Vector2f> enemyBound = {
    { EnemyType::Train_attendant, { 213, 377 } },
    { EnemyType::LightMonster, { 184, 376 } },
    { EnemyType::TicketMonster, { 176, 294 } }
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

        int sum_health; // 总生命值
        int cur_health; // 当前生命值
        std::vector<Plan> allPlans; // 下一回合计划（0-攻击，1-防御，2-增益，3-减益）
        std::vector<StatusType> StatusEffect; // 状态效果（如中毒、虚弱等）

    public:
        Enemy(EnemyType id, sf::Vector2f p) : id(id), position(p), bound({p, enemyBound.at(id)}){}
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