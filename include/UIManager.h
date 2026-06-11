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
    DiscardPilePanel discardPilePanel;

    bool hasFont = false;
    const sf::Font* font = nullptr;

public:
    UIManager();
    ~UIManager();

    bool Init(ResourceManager& rm);
    
    std::vector<GameEvent>& GetEvents(){ return events; }

    SettingPanel& GetSettingsPanel(){ return settingPanel; }

    bool HandleMousePressed(const sf::Vector2f& mousePos);
    void HandleMouseMoved(const sf::Vector2f& mousePos);
    void HandleMouseReleased();

    void DrawPanels(sf::RenderWindow& window, sf::Vector2i& mousePos){ 
        settingPanel.Draw(window); 
        backpackPanel.Draw(window, mousePos);
        discardPilePanel.Draw(window, mousePos);
    }

    void OpenSettingsPopup(){ settingPanel.Open(); }
    bool IsSettingsPopupOpen(){ return settingPanel.IsVisible(); }
    void CloseSettingsPopup(){ settingPanel.Close(); }

    void OpenBackpackPopup(){ backpackPanel.Open(); }
    bool IsBackpackPopupOpen(){ return backpackPanel.IsVisible(); }
    void CloseBackpackPopup(){ backpackPanel.Close(); }
    void SetBackPackCard(std::vector<PileType>& cards){ backpackPanel.SetCards(cards); }
    
    void OpenDiscardPopup(){ discardPilePanel.Open(); }
    bool IsDiscardPopupOpen(){ return discardPilePanel.IsVisible(); }
    void CloseDiscardPopup(){ discardPilePanel.Close(); }
    void SetDiscardCard(std::vector<PileType>& cards){ discardPilePanel.SetCards(cards); }

    void scaleToWindow(sf::Sprite& sprite)
    {
        const auto size = sprite.getTexture().getSize();
        sprite.setScale({
            static_cast<float>(WINDOW_WIDTH) / static_cast<float>(size.x),
            static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(size.y)
        });
    }
};
