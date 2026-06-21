#pragma once

#include "Constants.h"
#include "Tools/ResourceManager.h"
#include "Panel.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <UI/AnimationManager.h>
#include <UI/RewardAnimation.h>
#include <UI/TextPromptManager.h>

struct Notification {
    std::optional<sf::Sprite> bg;
    std::optional<sf::Sprite> icon;
    sf::Text text;

    Notification(const sf::Font& font)
        : text(font)
    {}

    float timer = 0.f;
    sf::Vector2f pos;
    float alpha = 255.f;

    enum class State {
        Entering,
        Stay,
        Exiting
    } state = State::Entering;
};

class UIManager {
private:
    ResourceManager* resourceManager = nullptr;
    std::vector<GameEvent> events;
    std::vector<Notification> notifications;

    PanelManager panels;

    const sf::Font* font = nullptr;

    void UpdateNotifications(float dt){
        for (auto& n : notifications){
            n.timer += dt;

            if (n.state == Notification::State::Entering){
                n.pos.x += 800.f * dt; // 从左滑入

                if (n.pos.x >= 50.f) {
                    n.pos.x = 50.f;
                    n.state = Notification::State::Stay;
                    n.timer = 0.f;
                }
            }
            else if (n.state == Notification::State::Stay){
                if (n.timer >= 2.0f) {
                    n.state = Notification::State::Exiting;
                }
            }
            else if (n.state == Notification::State::Exiting) {
                n.pos.y -= 200.f * dt;  // 向上飘

                n.alpha -= 300.f * dt;

                if (n.alpha <= 0.f) {
                    n.alpha = 0.f;
                    n.state = Notification::State::Stay; // 标记删除
                }
            }
        }

        // 删除结束通知
        notifications.erase(
        std::remove_if(notifications.begin(), notifications.end(),
            [](const Notification& n) {
                return n.alpha <= 0.f;
            }),
            notifications.end()
        );
    }
public:
    AnimationManager aniMgr;
    RewardAnimation rewardAni{aniMgr};
    TextPromptManager textPrompt{aniMgr};

    UIManager(){}
    std::vector<GameEvent>& GetEvents(){ return events; }

    void Init(ResourceManager& rm){
        resourceManager = &rm;
        font = &rm.getFont();

        panels.AddPanel<SettingPanel>(events)->Init(rm, font);
        panels.AddPanel<BackpackPanel>(events)->Init(rm, font);
        panels.AddPanel<DiscardPilePanel>(events)->Init(rm, font);
        panels.AddPanel<DealCardPanel>(events)->Init(rm, font);
        panels.AddPanel<CardsInHandPanel>(events)->Init(rm, font);
    }

    void Update(float dt){
        panels.Update(dt);
        UpdateNotifications(dt);
        aniMgr.Update(dt);
        textPrompt.Update(dt);
    }

    #pragma region 面板
    bool BlockInput(){ return panels.BlocksInput(); }

    bool HandleMousePressed(const sf::Vector2f& pos) { return panels.HandleMousePressed(pos); }
    bool HandleMouseMoved(const sf::Vector2f& pos) { return panels.HandleMouseMoved(pos); }
    bool HandleMouseReleased() { return panels.HandleMouseReleased(); }

    void DrawPanels(sf::RenderWindow& window){ panels.Draw(window); }

    void Open(PanelType type){ panels.Open(type); }
    void Close(PanelType type){ panels.Close(type); }
    void CloseAll(){ panels.CloseAll(); }
    bool IsOpen(PanelType type){ return panels.IsOpen(type); }
    template<typename T>
    T* Get() { return panels.Get<T>(); }
    #pragma endregion

    void PushNotification(const std::string& text, TextureType icon){
        notifications.emplace_back(*font);
        Notification& n = notifications.back();

        n.text.setFont(*font);
        n.text.setString(text);
        n.text.setCharacterSize(24);

        n.icon.emplace(resourceManager->getTexture(icon));
        n.bg.emplace(resourceManager->getTexture(TextureType::NotificationBG));
        n.bg->setScale({0.7, 0.7});
        if(icon == TextureType::Key){
            n.icon->setScale({0.55, 0.55});
        }
        else{
            n.icon->setScale({0.18, 0.18});
        }

        n.pos = {-400.f, 300.f}; // 左侧外面起点
        n.state = Notification::State::Entering;

        //notifications.push_back(n);
    }

    void DrawNotifications(sf::RenderWindow& window){
        for (auto& n : notifications){
            sf::Color c(255,255,255,static_cast<std::uint8_t>(n.alpha));

            n.bg->setPosition(n.pos);
            n.icon->setPosition(n.pos + sf::Vector2f(30,20));
            n.text.setPosition(n.pos + sf::Vector2f(110,40));

            n.bg->setColor(c);
            n.icon->setColor(c);
            n.text.setFillColor(c);

            window.draw(*n.bg);
            window.draw(*n.icon);
            window.draw(n.text);
        }
    }

};
