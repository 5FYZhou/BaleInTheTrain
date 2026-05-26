#include "Game.h"

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <thread>

Game::Game(): renderer(rm){
    // 初始化游戏窗口
    windowWidth = 1920;
    windowHeight = 1080;
	//设定窗口属性，窗口禁止缩放
	uint8_t WindowStyle = sf::Style::Close | sf::Style::Titlebar;
	window.create(sf::VideoMode({
            static_cast<unsigned int>(windowWidth),
            static_cast<unsigned int>(windowHeight)
        }), L"BaleInTheTrain", WindowStyle);
    
    // 初始化计时器
    timeSystem = TimeSystem();
    // 初始化渲染器
    renderer.Init();
}

void Game::Run(){
    Init();
    const int targetFPS = 60;
    const int frameDelay = 1000 / targetFPS; // ms
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        HandleInput();
        Logic();
        Draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
    }
}

void Game::Init(){
}

void Game::Logic(){
}

void Game::HandleInput(){
    MouseState mouse = input.GetMouseState(window);
    InputCommand cmd = input.GetCommand();
    switch (cmd)
    {
    case InputCommand::Mouse0Down:
        // 处理鼠标左键按下
        break;
    case InputCommand::MoveLeft:
        // 玩家向左移动
        //player.Move(-1);
        break;
    case InputCommand::MoveRight:
        //player.Move(1);
        break;
    default:
        break;
    }

    if(input.KeyDown(MoveLeft)){
        player.Move(-1);
    }
    else if(input.KeyDown(MoveRight)){
        player.Move(1);
    }
}

void Game::Draw(){
    window.clear();
    renderer.DrawPlayer(window, player.GetPos());
    renderer.DrawCard(window, scene.GetCardsInOneScene(0));
    window.display();
}