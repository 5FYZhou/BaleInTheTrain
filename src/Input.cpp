#include "Input.h"
#include <conio.h>
#include <windows.h>
#include <iostream>

Input::Input() {
    // 构造函数，如果需要初始化可以在这里
}

MouseState Input::GetMouseState(sf::RenderWindow& window) const {
    MouseState state;
    // 获取鼠标状态的逻辑
     state.position = sf::Mouse::getPosition(window);

    return state;
}