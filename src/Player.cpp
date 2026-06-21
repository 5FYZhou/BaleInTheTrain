#include "Player.h"

#include <algorithm>

namespace {

int GetCardCost(PileType type)
{
    switch (type)
    {
    case PileType::Anger:
    case PileType::Activate_muscles:
    case PileType::Sacrifice:
        return 0;
    case PileType::Heavy_strike:
    case PileType::Unstoppable:
        return 2;
    case PileType::Rampart:
        return 3;
    default:
        return 1;
    }
}

} // namespace

Player::Player(){}

Player::~Player() = default;

void Player::Init(int frameCountValue)
{
    frameCount = frameCountValue;
    SetSpeed(430.f);
    ResetExceptCard();
    SetHeight(PlayerHeight);
}

void Player::ResetExceptCard(){
    SetFeet({PlayerStartX, PlayerGroundY});
    SetFacing(1);
    ResetToStand();
    currentHP = 100;
}

void Player::InitCards()
{
    cards.clear();
    for (int i = 0; i < 5; ++i)
        cards.emplace_back(i, PileType::Strike, 1);

    for (int i = 5; i < 10; ++i)
        cards.emplace_back(i, PileType::Defend, 1);
}

void Player::AddCards(PileType type)
{
    const int id = static_cast<int>(cards.size());
    cards.emplace_back(id, type, GetCardCost(type));
}

void Player::Move(int direction, float dt, bool canTranslate)
{
    if (direction == 0)
    {
        ResetToStand();
        isMoving = false;
        return;
    }

    if (canTranslate)
        feet.x += speed * direction * dt;

    SetFacing(direction);
    UpdateFrame(dt);
    isMoving = true;
}

void Player::ClampPosition(float minX, float maxX)
{
    feet.x = std::clamp(feet.x, minX, maxX);
    feet.y = groundY;
}

void Player::UpdateFrame(float dt)
{
    walkTimer += dt;
    if (walkTimer >= frameSpeed)
    {
        walkTimer = 0.f;
        textureIndex = (textureIndex + 1) % frameCount;
    }
}

void Player::SetHP(int current, int max)
{
    maxHP = std::max(1, max);
    currentHP = std::clamp(current, 0, maxHP);
}
