#pragma once

#include "Constants.h"
#include "DialogManager.h"
#include "ResourceManager.h"
#include "Panel.h"
#include <SFML/Graphics.hpp>

class UIManager {
private:
    ResourceManager* resourceManager = nullptr;
    std::vector<GameEvent> events;
    SettingPanel settingPanel;
    BackpackPanel backpackPanel;

    sf::Sprite* statusBox = nullptr;
    sf::Sprite* potionIcon1 = nullptr;
    sf::Sprite* potionIcon2 = nullptr;
    sf::Sprite* potionIcon3 = nullptr;
    sf::Sprite* cubeIcon = nullptr;
    sf::Sprite* discardPileIcon = nullptr;
    sf::Sprite* dialogBox = nullptr;

    sf::RectangleShape hpBack;
    sf::RectangleShape hpBar;
    sf::Text* hpText = nullptr;
    sf::Text* dialogText = nullptr;
    sf::Text* dialogHintText = nullptr;
    sf::Text* movementHintText = nullptr;

    int currentHP = 100;
    int maxHP = 100;
    bool hasFont = false;
    sf::Font font;

    float SliderValueFromX(float x, float left, float width) const;
    void DrawSlider(sf::RenderWindow& window, const std::string& label, float value, sf::Vector2f pos);
    void DrawCard(sf::RenderWindow& window, const CardView& card, float alpha);
    void ApplyDisabledIconColor(sf::Sprite* sprite);

public:
    UIManager();
    ~UIManager();

    bool Init(ResourceManager& rm);
    bool LoadFont();

    void SetHP(int current, int max);
    void TakeDamage(int damage);
    void Heal(int amount);
    int GetCurrentHP() const { return currentHP; }
    int GetMaxHP() const { return maxHP; }
    std::vector<GameEvent>& GetEvents(){ return events; }

    SettingPanel& GetSettingsPanel(){ return settingPanel; }

    bool HandleMousePressed(const sf::Vector2f& mousePos);
    void HandleMouseMoved(const sf::Vector2f& mousePos);
    void HandleMouseReleased();

    void DisablePotionIcons();
    void EnablePotionIcon(int index);

    void Draw(sf::RenderWindow& window);
    void DrawDialog(sf::RenderWindow& window, const DialogManager& dialogMgr);
    void DrawMovementHint(sf::RenderWindow& window);
    void DrawCardRewards(sf::RenderWindow& window, const std::vector<CardType>& cardsV, float alpha);
    void DrawPanels(sf::RenderWindow& window, sf::Vector2i& mousePos){ 
        settingPanel.Draw(window); 
        backpackPanel.Draw(window, mousePos);
    }

    void OpenSettingsPopup(){ settingPanel.Open(); }
    bool IsSettingsPopupOpen(){ return settingPanel.IsVisible(); }
    void CloseSettingsPopup(){ settingPanel.Close(); }

    void OpenBackpackPopup(){ backpackPanel.Open(); }
    bool IsBackpackPopupOpen(){ return backpackPanel.IsVisible(); }
    void CloseBackpackPopup(){ backpackPanel.Close(); }
    void SetBackPackCard(std::vector<CardType>& cards){ backpackPanel.SetCards(cards); }

    sf::Sprite* GetStatusBox() const { return statusBox; }

    void scaleToWindow(sf::Sprite& sprite)
    {
        const auto size = sprite.getTexture().getSize();
        sprite.setScale({
            static_cast<float>(WINDOW_WIDTH) / static_cast<float>(size.x),
            static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(size.y)
        });
    }
};
