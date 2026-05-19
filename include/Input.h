#pragma once

#include "Constants.h"
#include <SFML/Graphics.hpp>

enum InputCommand { None, Default, Mouse0Down, Mouse0Up, Mouse1Down, Mouse1Up, 
    Mouse0DownDouble, Mouse0UpDouble,
    MoveLeft, MoveRight,
    Confirm, Cancel };

struct MouseState {
    bool leftDown = false;
    bool rightDown = false;
    sf::Vector2i position;
};

class Input {
public:
    Input();

    InputCommand GetCommand() const;
    MouseState GetMouseState(sf::RenderWindow& window) const;
};