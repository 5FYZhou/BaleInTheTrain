#include "UIManager.h"

UIManager::UIManager() : settingPanel(events), backpackPanel(events){
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
    backpackPanel.Close();
    scaleToWindow(*backpackPanel.background);

    settingPanel.Init(rm, font);
    backpackPanel.Init(rm, font);
    return true;
}

bool UIManager::HandleMousePressed(const sf::Vector2f& mousePos){
    if(settingPanel.IsVisible()){
        return settingPanel.HandleMousePressed(mousePos);
    }
    if(backpackPanel.IsVisible()){
        return backpackPanel.HandleMousePressed(mousePos);
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
