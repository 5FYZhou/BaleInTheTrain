#pragma once
#include "Constants.h"
class Card{
public:
    int id; // 卡牌ID
    PileType name; // 卡牌名称
    //std::string description; // 卡牌描述
    int cost; // 卡牌费用

    int damage; // 伤害值（如果是攻击牌）
    int block; // 防御值（如果是防御牌）


    Card(int id, const PileType& name, const  int cost)
        : id(id), name(name), cost(cost) {}

    void triggerEffect(Player &player);
    void triggerEffect(Enemy &enemy);
};