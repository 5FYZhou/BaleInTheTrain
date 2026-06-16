#include "Player.h"
#include <cmath>
#include <algorithm>

Player::Player() 
    : feet(350.f, 1028.f), height(454.f), groundY(1028.f) {
}

Player::~Player() {
}

void Player::Init(int fc){
    frameCount = fc;
    SetFeet({PlayerStartX, PlayerGroundY});
    SetFacing(1);
    SetHeight(PlayerHeight);
    SetSpeed(430.f);
    //player.SetFrame(PlayerFrame::Stand);
    SetTextureIndex(0);
}
void Player::InitCards(){
    for(int i = 0;i<5;++i){
        cards.push_back({i,PileType::Strike,1});
    }
    for(int i = 5; i < 10;++i){
        cards.push_back({i,PileType::Defend,1});
    }
}

void Player::Move(int direction, float dt, bool canTranslate) {
    if(direction == 0){
        ResetToStand();
        isMoving = false;
        return;
    }
    if(canTranslate)
        feet.x += speed * direction * dt;
    SetFacing(direction);
    UpdateFrame(dt);
    isMoving = true;
}

void Player::ClampPosition(float minX, float maxX) {
    feet.x = std::min(std::max(feet.x, minX), maxX);
    feet.y = groundY;
}

void Player::UpdateFrame(float dt) {
    walkTimer += dt;
    if(walkTimer >= frameSpeed){
        walkTimer = 0.f;
        textureIndex = (textureIndex + 1) % frameCount; // Cycle through 3 frames
    }
}

void Player::SetHP(int current, int max){
    currentHP = std::clamp(current, 0, std::max(1, max));
    maxHP = std::max(1, max);
}

void Player::TakeDamage(int damage)
{
    SetHP(currentHP - damage, maxHP);
}

void Player::Heal(int amount)
{
    SetHP(currentHP + amount, maxHP);
}