#include "Input.h"
#include <conio.h>
#include <windows.h>
#include <iostream>

Input::Input() {
    // 构造函数，如果需要初始化可以在这里
}

InputCommand Input::GetCommand() const {
    // 这里可以实现获取输入命令的逻辑，例如根据键盘输入返回不同的命令
    // 目前暂时返回 None，后续可以根据实际需求进行扩展
    return InputCommand::None;
}

MouseState Input::GetMouseState(sf::RenderWindow& window) const {
    MouseState state;
    // 获取鼠标状态的逻辑
     state.position = sf::Mouse::getPosition(window);

    return state;
}