#include "UIManager.h"

UIManager::UIManager() : 
    settingPanel(events), backpackPanel(events), discardPilePanel(events){
}

UIManager::~UIManager(){
    delete font;
}

bool UIManager::Init(ResourceManager& rm){
    resourceManager = &rm;
    font = &rm.getFont();
    hasFont = font;

    settingPanel.panel.emplace(rm.getTexture(TextureType::SettingsPanel));
    settingPanel.closeButton.emplace(rm.getTexture(TextureType::CloseButton));

    backpackPanel.backButton.emplace(rm.getTexture(TextureType::BackButton));
    backpackPanel.background.emplace(rm.getTexture(TextureType::BackpackInterior));
    scaleToWindow(*backpackPanel.background);

    discardPilePanel.backButton.emplace(rm.getTexture(TextureType::BackButton));

    settingPanel.Init(rm, font);
    backpackPanel.Init(rm, font);
    discardPilePanel.Init(rm, font);
    return true;
}

bool UIManager::HandleMousePressed(const sf::Vector2f& mousePos){
    if(settingPanel.IsVisible()){
        return settingPanel.HandleMousePressed(mousePos);
    }
    if(backpackPanel.IsVisible()){
        return backpackPanel.HandleMousePressed(mousePos);
    }
    if(discardPilePanel.IsVisible()){
        return discardPilePanel.HandleMousePressed(mousePos);
    }

    return false;
}

void UIManager::HandleMouseMoved(const sf::Vector2f& mousePos){
    if(settingPanel.IsVisible()){
        settingPanel.HandleMouseMoved(mousePos);
    }
}

void UIManager::HandleMouseReleased(){
    if(settingPanel.IsVisible()){
        settingPanel.HandleMouseReleased();
    }
}
