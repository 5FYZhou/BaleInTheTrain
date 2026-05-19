#pragma once

#include "Constants.h"
#include <SFML/Graphics.hpp>

enum InputCommand { None, Default, Mouse0Down, Mouse0Up, Mouse1Down, Mouse1Up, Mouse0DownDouble, Mouse0UpDouble,
Confirm, Cancel };

struct MouseState {
    bool leftDown = false;
    bool rightDown = false;
    sf::Vector2i position;
    InputCommand command = InputCommand::None;
};

class Input {
public:
    Input();

    MouseState GetMouseState(sf::RenderWindow& window) const;
};