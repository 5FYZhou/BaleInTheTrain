#include "Input.h"
#include <conio.h>
#include <windows.h>
#include <iostream>

Input::Input() {
    // 构造函数，如果需要初始化可以在这里
}

InputCommand Input::GetCommand() const {
    InputCommand cmd = None;
    while (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            // 方向键、功能键等特殊键返回两字节序列
            int special = _getch();
            switch (special) {
            case 72:  break;    // ↑
            case 80:  break;  // ↓
            case 75:  break;  // ←
            case 77:  break; // →
            default: cmd = Default; break;
            }
            continue;
        }

        switch (ch) {
        case 'a': case 'A': cmd = MoveLeft; break;
        case 'd': case 'D': cmd = MoveRight; break;
        case 'y': case 'Y': cmd = Confirm; break;
        // Ctrl + WASD 可能产生控制字符
        case 23:  break;    // Ctrl+W
        case 19:  break;  // Ctrl+S
        case 1:   break;  // Ctrl+A
        case 4:   break; // Ctrl+D
        // ESC
        case 27: cmd = Exit; break;
        // Space
        case 32: cmd = Pause; break;
        // Enter
        case 13: break;
        default: cmd = Default; break;
        }
    }
    return cmd;  // 返回处理后的命令
}

InputCommand Input::GetMouseCommand(sf::RenderWindow& window) const {
    InputCommand cmd = None;
    using namespace std::chrono;
    auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    const int DOUBLE_CLICK_TIME = 300; // ms
    const int DOUBLE_CLICK_DIST = 5; // px

    MouseState state = GetMouseState(window);
    // 检测左键单击
    if (state.leftDown && !lastLeftDown) {
        // 判断与上次点击的时间和距离
        if (now - lastLeftClickTime < DOUBLE_CLICK_TIME &&
            std::abs(state.position.x - lastClickPos.x) <= DOUBLE_CLICK_DIST &&
            std::abs(state.position.y - lastClickPos.y) <= DOUBLE_CLICK_DIST) {
            cmd = Mouse0DownDouble;
            lastLeftClickTime = 0; // 重置，防止连击
        } else {
            cmd = Mouse0Down;
            lastLeftClickTime = now;
            lastClickPos = state.position;
        }
    } else {
        cmd = InputCommand::None;
    }
    lastLeftDown = state.leftDown;

    if(state.rightDown && !lastRightDown){
        if(now - lastRightClickTime < DOUBLE_CLICK_TIME &&
            std::abs(state.position.x - lastClickPos.x) <= DOUBLE_CLICK_DIST &&
            std::abs(state.position.y - lastClickPos.y) <= DOUBLE_CLICK_DIST) {
            cmd = Mouse1DownDouble;
            lastRightClickTime = 0; // 重置，防止连击
        } else {
            cmd = Mouse1Down;
            lastRightClickTime = now;
            lastClickPos = state.position;
        }
    }
    lastRightDown = state.rightDown;
    return cmd;
}

MouseState Input::GetMouseState(sf::RenderWindow& window) const {
    MouseState state;
    // 获取鼠标状态的逻辑
    state.position = sf::Mouse::getPosition(window);
    state.leftDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    state.rightDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    return state;
}

bool Input::GetOneKey(InputCommand cmd) {
    int ch = _getch();
    switch (cmd) {
        case Pause:
            if (ch == 32) return true; // Space
            break;
        case Confirm:
            if (ch == 'Y' || ch == 'y') return true;
            break;
        default:
            break;
    }
    return false;
}

bool Input::KeyDown(InputCommand cmd){
    switch (cmd)
    {
    case Sprint:
        return (GetAsyncKeyState(VK_MENU) & 0x8000);
    case MoveLeft:
        return (GetAsyncKeyState('A') & 0x8000);
    case MoveRight:
        return (GetAsyncKeyState('D') & 0x8000);
    default:
        break;
    }
    return false;
}

InputCommand Input::GInput(){
    // 先处理退出 / 功能键（优先级高）
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return Exit;

    static SHORT lastSpaceState = 0;
    SHORT now = GetAsyncKeyState(VK_SPACE);
    // 检测“刚按下”
    if ((now & 0x8000) && !(lastSpaceState & 0x8000)) {
        lastSpaceState = now;
        return Pause;
    }
    lastSpaceState = now;

    
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) ;
    if((GetAsyncKeyState('Y') & 0x8000)) return Confirm;

    // 方向键（支持 WASD + 方向键）
    if ((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000)) {
    }
    if ((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000)) {
    }
    if ((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000)) {
    }
    if ((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000)) {
    }
    return None;
}

bool Input::WaitAnyKey(){
    if(_getch()) return true;
    return false;
}