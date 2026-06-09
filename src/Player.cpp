#include "Player.h"
#include <cmath>
#include <algorithm>

Player::Player() 
    : feet(350.f, 1028.f), height(454.f), groundY(1028.f) {
}

Player::~Player() {
}

void Player::Move(int direction, float dt) {
    feet.x += speed * direction * dt;
    UpdateFrame(dt);
}

void Player::ClampPosition(float minX, float maxX) {
    feet.x = std::min(std::max(feet.x, minX), maxX);
    feet.y = groundY;
}

void Player::UpdateFrame(float dt) {
    walkTimer += dt;
    if(walkTimer >= frameSpeed){
        walkTimer = 0.f;
        textureIndex = (textureIndex + 1) % 3; // Cycle through 3 frames
    }
}

void Player::UpdateSpritePosition() {
    // Player stores feet position; rendering happens in Renderer.
}
