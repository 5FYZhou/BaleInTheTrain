#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Card.h"

class Player {
public:
    // Position and movement
    sf::Vector2f feet;          // Player's foot position (ground level)
    int facing = 1;             // Direction: 1 = right, -1 = left
    float speed = 430.f;        // Movement speed in pixels per second
    float nextX;

    int currentHP = 100;
    int maxHP = 100;

    // Animation
    float walkTimer = 0.f;      // Timer for walking animation
    bool isMoving = false;
    float frameSpeed = 0.0625f;        // Frames per second for walking animation
    int textureIndex = 0;
    int frameCount;

    // Texture references (Player does not own sprites)
    float height = 454.f;       // Player sprite height in pixels
    float groundY = 1028.f;     // Ground level Y coordinate

    sf::FloatRect bound;//{ 195.f, 330.f };

public:
    Player();
    ~Player();

    //战斗相关数值
    std::vector<Card> cards;

    void Init(int fc);
    void InitCards();
    void AddCards(PileType p);
    void ResetExceptCard();

    // Getters
    sf::Vector2f GetFeet() const { return feet; }
    sf::Vector2f GetPos() const { return sf::Vector2f(feet.x, feet.y - height); }
    int GetFacing() const { return facing; }
    bool GetIsMoving() const { return isMoving; }
    float GetWalkTimer() const { return walkTimer; }
    float GetHeight() const { return height; }
    int GetTextureIndex() const { return textureIndex; }
    int GetCurrentHP() const { return currentHP; }
    int GetMaxHP() const { return maxHP; }
    float GetNextX(int dir, float dt) const { return feet.x + speed * dir * dt;; }
    std::vector<PileType> GetPileCards() const {
        std::vector<PileType> pv;
        for(const auto& c : cards){
            pv.push_back(c.name);
        }
        return pv;
    }

    // Setters
    void SetFeet(sf::Vector2f newFeet) { feet = newFeet; }
    void SetFacing(int newFacing) { facing = newFacing; }
    void SetMoving(bool moving) { isMoving = moving; }
    void SetGroundY(float y) { groundY = y; }
    void SetHeight(float h) { height = h; }
    void SetSpeed(float s) { speed = s; }
    void SetTextureIndex(int index) { textureIndex = index; }
    void SetHP(int current, int max);
    
    // Movement
    void Move(int direction, float dt, bool canTranslate = true);
    void ClampPosition(float minX, float maxX);

    // Animation
    void UpdateFrame(float dt);
    void ResetToStand() { walkTimer = 0.f; textureIndex = 0; }
    void UpdateWalkTimer(float dt) { walkTimer += dt; }
};