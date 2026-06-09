#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"

class Player {
private:
    // Position and movement
    sf::Vector2f feet;          // Player's foot position (ground level)
    int facing = 1;             // Direction: 1 = right, -1 = left
    float speed = 430.f;        // Movement speed in pixels per second

    // Animation
    float walkTimer = 0.f;      // Timer for walking animation
    bool isMoving = false;
    float frameSpeed = 0.0625f;        // Frames per second for walking animation
    int textureIndex = 0;

    // Texture references (Player does not own sprites)
    float height = 454.f;       // Player sprite height in pixels
    float groundY = 1028.f;     // Ground level Y coordinate

public:
    Player();
    ~Player();

    // Getters
    sf::Vector2f GetFeet() const { return feet; }
    sf::Vector2f GetPos() const { return sf::Vector2f(feet.x, feet.y - height); }
    int GetFacing() const { return facing; }
    bool GetIsMoving() const { return isMoving; }
    float GetWalkTimer() const { return walkTimer; }
    float GetHeight() const { return height; }
    int GetTextureIndex() const { return textureIndex; }

    // Setters
    void SetFeet(sf::Vector2f newFeet) { feet = newFeet; }
    void SetFacing(int newFacing) { facing = newFacing; }
    void SetMoving(bool moving) { isMoving = moving; }
    void SetGroundY(float y) { groundY = y; }
    void SetHeight(float h) { height = h; }
    void SetSpeed(float s) { speed = s; }
    void SetTextureIndex(int index) { textureIndex = index; }

    // Movement
    void Move(int direction, float dt);
    void ClampPosition(float minX, float maxX);

    // Animation
    void UpdateFrame(float dt);
    void ResetToStand() { walkTimer = 0.f; textureIndex = 0; }
    void UpdateWalkTimer(float dt) { walkTimer += dt; }

    // Physics helpers
    void UpdateSpritePosition();
};