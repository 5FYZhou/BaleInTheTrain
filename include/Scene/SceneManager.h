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
    int curGameIdx = 0;
    BattleScene battleScene{events};
    WinScene winScene{events};
    DeadScene deadScene{events};

    Scene* curScene;
    SceneType pendingScene = SceneType::Menu;
    float pendingPlayerX = PlayerStartX;

    FadeState fadeState = FadeState::None;
    float fadeAlpha = 0.f;
    static constexpr float FadeSpeed = 520.f;

    int gameIdxBeforeBattle;
    
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
    void Init(){
        gameScene1.Init();
        gameScene2.Init();
        gameScene3.Init();
    }
    
    Scene& GetScene() { return *curScene; }
    const Scene& GetScene() const { return *curScene; }
    SceneType GetCurSceneType() const { return curScene->GetType(); }
    bool IsFading() const { return fadeState != FadeState::None; }
    float GetFadeAlpha() const { return fadeAlpha; }
    FadeState GetFadeState() const { return fadeState; }
    std::vector<GameEvent>& GetEvents() { return events; }
    int GetGameSceneIdx() { return curGameIdx; }

    void SetCurScene(SceneType type, int idx = 0, float playerX = PlayerStartX) {
        switch (type)
        {
        case SceneType::Menu:
            curScene = &menuScene;
            break;
        case SceneType::Game:
        {
            Scene* lastScene = curScene;
            if(idx == 0){
                curScene = &gameScene1;
            } 
            else if(idx == 1){
                curScene = &gameScene2;
            } 
            else if(idx == 2){
                curScene = &gameScene3;
            }
            if(lastScene->GetType() == SceneType::Battle){
                curScene->EnemyDrop();
            }
            else{
                events.push_back({EventType::ResetPlayerPos, playerX});
            }
        }
            break;
        case SceneType::Battle:
            battleScene.SetEnemy(curScene->GetClickEnemy());
            curScene = &battleScene;
            events.push_back({EventType::ResetPlayerPos, BattleX});
            break;
        case SceneType::Win:
            curScene = &winScene;
            break;
        case SceneType::Dead:
            curScene = &deadScene;
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
        gameIdxBeforeBattle = curGameIdx;
        pendingScene = type;
        curGameIdx = 0;
        pendingPlayerX = PlayerStartX;
        callback = cb;
        StartFadeOut();
    }

    void LoadScene(SceneType type, int idx, std::function<void()> cb = nullptr) {
        pendingScene = type;
        curGameIdx = idx;
        callback = cb;
        StartFadeOut();
    }

    void Update(float dt) {
        curScene->Update(dt);

        if (fadeState == FadeState::FadeOut) {
            fadeAlpha += FadeSpeed * dt;
            if (fadeAlpha >= 255.f) {
                fadeAlpha = 255.f;
                SetCurScene(pendingScene, curGameIdx, pendingPlayerX);
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

    bool CheckChangeGameScene(float playerX, int keyCnt){
        if(curScene->GetType() == SceneType::Game){
            // 如果角色位置大于左/右边界
            if(playerX <= LeftExitX) {
                pendingPlayerX = RightEntryX;
                return changeToNextGameScene(-1, keyCnt);
            } 
            else if(playerX >= RightExitX) {
                pendingPlayerX = LeftEntryX;
                return changeToNextGameScene(1, keyCnt);
            }
        }
        return true;
    }

    bool changeToNextGameScene(int offset, int keyCnt){
        int next = curGameIdx + offset;
        if(next < 0 || next > 2) {
            GameEvent event;
            if(keyCnt >= KeyCntToOpenDoor)
                event.type = EventType::Win;
            else
                event.type = EventType::KeysInsufficient;
            events.push_back(event);
            GameEvent e;
            e.type = EventType::ResetPlayerPos;
            e.val = next < 0 ? LeftExitX + 1 : RightExitX - 1;
            events.push_back(e);
            // 撞墙
            return false;
        }
        else{
            LoadScene(SceneType::Game, next);
            return true;
        }
    }

    void LoadGameBeforeBattle(std::function<void()> cb = nullptr){
        LoadScene(SceneType::Game, gameIdxBeforeBattle, cb);
    }

    void AddGhost(int idx, sf::Vector2f pos, int keyNum){
        GameScene* sc = nullptr;
        if(idx == 0){ sc = &gameScene1; }
        else if(idx == 1){ sc = &gameScene2; }
        else{ sc = &gameScene3; }

        Enemy ghost(EnemyType::Past_YOU, pos, {0.5f, 0.5f});
        for(int i = 0; i < keyNum; i++){
            ghost.droppedItems.push_back( ItemType::Key );
        }
        ghost.frameCount = 1;
        ghost.dropKeyNum = keyNum;
        sc->AddGhost(ghost);
    }

    void LoadFirstDie(std::function<void()> cb = nullptr){
        LoadScene(SceneType::Game, 0, cb);
    }

};
