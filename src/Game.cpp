#include "Game.h"

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <ctime>

Game::Game():gridSpriteSize(GRIDSIZE), renderer(rm, gridSpriteSize){
    // 初始化游戏窗口
    windowWidth = 860;
    windowHeight = 600;
	//设定窗口属性，窗口禁止缩放
	uint8_t WindowStyle = sf::Style::Close | sf::Style::Titlebar;
	window.create(sf::VideoMode({
            static_cast<unsigned int>(windowWidth),
            static_cast<unsigned int>(windowHeight)
        }), L"MineSweeper", WindowStyle);

    // 计算按钮位置
	BtnDetaX = (windowWidth - BTN_WIDTH * 7) / 8;  //7个按钮在界面上等分宽度
	BtnLeftCorner = {BtnDetaX, windowHeight - GRIDSIZE * 3};		//指定高度

    timeSystem = TimeSystem();
    intervalTime = 0.5f;
    intervalTimer = 0.0f;
    gameState = GameState::MENU;
    diff = Diff::EASY;

    gridWidth = WIDTH;
    gridHeight = HEIGHT;
    //grid = std::vector<GridType>(gridWidth * gridHeight, GridType::DEFAULT);

    renderer.Init();
}

void Game::Run(){
    Init();
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        HandleInput();
        Logic();
        Draw();
    }
}

void Game::Init(){
    // 初始化按钮
    int x = BtnDetaX;
    for(int i = 0; i < 7; i++){
        ButtonType type = static_cast<ButtonType>(i);
        sf::IntRect rect({x, BtnLeftCorner.y}, sf::Vector2i(BTN_WIDTH, BTN_HEIGHT));
        btns.emplace_back(type, rect);
        x += BtnDetaX + BTN_WIDTH;
    }
}

void Game::Logic(){
}

void Game::HandleInput(){
    MouseState mouse = input.GetMouseState(window);
    for(auto& b : btns){
        if(b.rect.contains(mouse.position)){
            b.state = ButtonState::Hover;
        }
        else{
            b.state = ButtonState::Default;
        }
    }
}

void Game::Draw(){
    window.clear();
    renderer.DrawBackground(window);
    renderer.DrawGrid(window, grid);
    renderer.DrawButton(window, BtnLeftCorner, BtnDetaX, btns);
    renderer.DrawScore(window, 100);
    renderer.DrawTimer(window, 123.0f);
    renderer.DrawGameEnd(window, true);
    window.display();
}