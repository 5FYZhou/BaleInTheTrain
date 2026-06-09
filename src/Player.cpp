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
    SetHeight(PlayerHeight);
    SetSpeed(430.f);
    //player.SetFrame(PlayerFrame::Stand);
    SetTextureIndex(0);
}

void Player::Move(int direction, float dt) {
    if(direction == 0){
        ResetToStand();
        isMoving = false;
        return;
    }
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
