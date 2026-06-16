#pragma once

#include "Constants.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

enum class FadeState { None, FadeOut, FadeIn };

class SceneManager {
private:
    std::vector<GameEvent> events;
    std::function<void()> callback;

    MenuScene menuScene{events};
    GameScene gameScene1{events, 0};
    GameScene gameScene2{events, 1};
    GameScene gameScene3{events, 2};
    BattleScene battleScene{events};
    int gameSceneIdx = 0;

    Scene* curScene;
    SceneType pendingScene = SceneType::Menu;
    FadeState fadeState = FadeState::None;
    float fadeAlpha = 0.f;
    static constexpr float FadeSpeed = 520.f;
    float pendingPlayerX = PlayerStartX;
    
    // Carriage boundaries and entry/exit positions
    static constexpr float LeftExitX = 210.f;      // Player exit on left
    static constexpr float RightExitX = 1710.f;    // Player exit on right
    static constexpr float LeftEntryX = 275.f;     // Entry position from left
    static constexpr float RightEntryX = 1645.f;   // Entry position from right
    static constexpr float BattleX = 275.f;   // Entry position of BattleScene

    void StartFadeOut() {
        fadeState = FadeState::FadeOut;
        fadeAlpha = 0.f;
    }

    void StartFadeIn() {
        fadeState = FadeState::FadeIn;
        fadeAlpha = 255.f;
    }

public:
    SceneManager(){
        curScene = &menuScene;
    };
    ~SceneManager() = default;
    
    Scene& GetScene() { return *curScene; }
    const Scene& GetScene() const { return *curScene; }
    SceneType GetCurSceneType() const { return curScene->GetType(); }
    bool IsFading() const { return fadeState != FadeState::None; }
    float GetFadeAlpha() const { return fadeAlpha; }
    FadeState GetFadeState() const { return fadeState; }
    std::vector<GameEvent>& GetEvents() { return events; }

    void SetCurScene(SceneType type, int idx = 0, float playerX = PlayerStartX) {
        switch (type)
        {
        case SceneType::Menu:
            curScene = &menuScene;
            break;
        case SceneType::Game:
            if(idx == 0){
                curScene = &gameScene1;
            } 
            else if(idx == 1){
                curScene = &gameScene2;
            } 
            else if(idx == 2){
                curScene = &gameScene3;
            }
            events.push_back({EventType::ResetPlayerPos, playerX});
            break;
        case SceneType::Battle:
            battleScene.SetEnemy(curScene->GetEnemy());
            curScene = &battleScene;
            events.push_back({EventType::ResetPlayerPos, BattleX});
            break;
        default:
            std::cout<<"SceneMgr:undefined load scene type"<<std::endl;
            break;
        }
        //pendingScene = type;
        fadeState = FadeState::None;
        fadeAlpha = 0.f;
        if(callback)
            callback();
    }

    void LoadScene(SceneType type, std::function<void()> cb = nullptr) {
        //if (fadeState != FadeState::None && pendingScene == type) {
        //    return;
        //}
        pendingScene = type;
        gameSceneIdx = 0;
        pendingPlayerX = PlayerStartX;
        callback = cb;
        StartFadeOut();
    }

    void LoadScene(SceneType type, int idx) {
        //if (fadeState != FadeState::None && pendingScene == type) {
        //    return;
        //}
        pendingScene = type;
        gameSceneIdx = idx;
        StartFadeOut();
    }

    void Update(float dt) {
        curScene->Update(dt);

        if (fadeState == FadeState::FadeOut) {
            fadeAlpha += FadeSpeed * dt;
            if (fadeAlpha >= 255.f) {
                fadeAlpha = 255.f;
                SetCurScene(pendingScene, gameSceneIdx, pendingPlayerX);
                StartFadeIn();
            }
        } 
        else if (fadeState == FadeState::FadeIn) {
            fadeAlpha -= FadeSpeed * dt;
            if (fadeAlpha <= 0.f) {
                fadeAlpha = 0.f;
                fadeState = FadeState::None;
            }
        }
    }

    void CheckChangeGameScene(float playerX){
        if(curScene->GetType() == SceneType::Game){
            // 如果角色位置大于左/右边界
            if(playerX <= LeftExitX) {
                pendingPlayerX = RightEntryX;
                changeToNextGameScene(-1);
            } 
            else if(playerX >= RightExitX) {
                pendingPlayerX = LeftEntryX;
                changeToNextGameScene(1);
            }
        }
    }

    void changeToNextGameScene(int offset){
        int next = gameSceneIdx + offset;
        if(next < 0 || next > 2) {
            LoadScene(SceneType::Menu);
        }
        else{
            LoadScene(SceneType::Game, next);
        }
    }
};
