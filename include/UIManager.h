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
    DealCardPanel dealCardPanel;
    CardsInHandPanel cardsInHandPanel;

    const sf::Font* font = nullptr;

    void scaleToWindow(sf::Sprite& sprite){
        const auto size = sprite.getTexture().getSize();
        sprite.setScale({
            static_cast<float>(WINDOW_WIDTH) / static_cast<float>(size.x),
            static_cast<float>(WINDOW_HEIGHT) / static_cast<float>(size.y)
        });
    }

public:
    UIManager() : settingPanel(events), backpackPanel(events)
        , discardPilePanel(events), dealCardPanel(events)
        , cardsInHandPanel(events){}
    ~UIManager(){ delete font; }

    void Init(ResourceManager& rm){
        resourceManager = &rm;
        font = &rm.getFont();

        settingPanel.panel.emplace(rm.getTexture(TextureType::SettingsPanel));
        settingPanel.closeButton.emplace(rm.getTexture(TextureType::CloseButton));

        backpackPanel.backButton.emplace(rm.getTexture(TextureType::BackButton));
        backpackPanel.background.emplace(rm.getTexture(TextureType::BackpackInterior));
        scaleToWindow(*backpackPanel.background);

        discardPilePanel.backButton.emplace(rm.getTexture(TextureType::BackButton));

        dealCardPanel.backButton.emplace(rm.getTexture(TextureType::BackButton));

        settingPanel.Init(rm, font);
        backpackPanel.Init(rm, font);
        discardPilePanel.Init(rm, font);
        dealCardPanel.Init(rm, font);
        cardsInHandPanel.Init(rm, font);
    }

    std::vector<GameEvent>& GetEvents(){ return events; }
    bool BlockInput(){ return settingPanel.IsVisible() || backpackPanel.IsVisible() 
        || discardPilePanel.IsVisible() || dealCardPanel.IsVisible(); }

    bool HandleMousePressed(const sf::Vector2f& mousePos){
        if(settingPanel.IsVisible()){
            return settingPanel.HandleMousePressed(mousePos);
        }
        if(backpackPanel.IsVisible()){
            return backpackPanel.HandleMousePressed(mousePos);
        }
        if(discardPilePanel.IsVisible()){
            return discardPilePanel.HandleMousePressed(mousePos);
        }
        if(dealCardPanel.IsVisible()){
            return dealCardPanel.HandleMousePressed(mousePos);
        }
        if(cardsInHandPanel.IsVisible()){
            return cardsInHandPanel.HandleMousePressed(mousePos);
        }
        return false;
    }
    void HandleMouseMoved(const sf::Vector2f& mousePos){
        if(settingPanel.IsVisible()){
            settingPanel.HandleMouseMoved(mousePos);
        }
        if(cardsInHandPanel.IsVisible()){
            cardsInHandPanel.HandleMouseMoved(mousePos);
        }
    }
    void HandleMouseReleased(){
        if(settingPanel.IsVisible()){
            settingPanel.HandleMouseReleased();
        }
    }

    void DrawPanels(sf::RenderWindow& window, sf::Vector2i& mousePos){ 
        cardsInHandPanel.Draw(window, mousePos);
        settingPanel.Draw(window); 
        backpackPanel.Draw(window, mousePos);
        discardPilePanel.Draw(window, mousePos);
        dealCardPanel.Draw(window, mousePos);
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

    void OpenDealCardPopup(){ dealCardPanel.Open(); }
    bool IsDealCardPopupOpen(){ return dealCardPanel.IsVisible(); }
    void CloseDealCardPopup(){ dealCardPanel.Close(); }
    void SetDealCardCard(std::vector<PileType>& cards){ dealCardPanel.SetCards(cards); }

    void OpenCardsInHandPopup(){ cardsInHandPanel.Open(); }
    bool IsCardsInHandPopupOpen(){ return cardsInHandPanel.IsVisible(); }
    void CloseCardsInHandPopup(){ cardsInHandPanel.Close(); }
    void SetCardsInHandCard(std::vector<PileType>& cards){ cardsInHandPanel.SetCards(cards); }
    bool HasSelectedCard(){ 
        if(cardsInHandPanel.IsVisible()) 
            return cardsInHandPanel.HasSelectedCard();
        return false;
    }
    void SetHasSelected(bool f){
        if(cardsInHandPanel.IsVisible()) 
            cardsInHandPanel.SetHasSelected(f);
    }
    std::pair<PileType, int> GetSelectedCard(){ return cardsInHandPanel.GetSelectedCard(); }

};
