#include "UIManager.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

UIManager::UIManager() : settingPanel(events), backpackPanel(events)
{
}

UIManager::~UIManager()
{
    delete statusBox;
    delete potionIcon1;
    delete potionIcon2;
    delete potionIcon3;
    delete cubeIcon;
    delete discardPileIcon;
    delete dialogBox;
    delete hpText;
    delete dialogText;
    delete dialogHintText;
    delete movementHintText;
}

bool UIManager::Init(ResourceManager& rm)
{
    resourceManager = &rm;
    hasFont = LoadFont();

    try {
        settingPanel.panel.emplace(rm.getTexture(TextureType::SettingsPanel));
        settingPanel.closeButton.emplace(rm.getTexture(TextureType::CloseButton));

        backpackPanel.backButton.emplace(rm.getTexture(TextureType::BackButton));
        backpackPanel.background.emplace(rm.getTexture(TextureType::BackpackInterior));
        backpackPanel.Close();
        scaleToWindow(*backpackPanel.background);

        statusBox = new sf::Sprite(rm.getTexture(TextureType::StatusBox));
        statusBox->setPosition({24.f, 24.f});

        potionIcon1 = new sf::Sprite(rm.getTexture(TextureType::Potion1));
        potionIcon1->setPosition({365.f, 30.f});
        potionIcon1->setScale({POTION_ICON_SCALE, POTION_ICON_SCALE});

        potionIcon2 = new sf::Sprite(rm.getTexture(TextureType::Potion2));
        potionIcon2->setPosition({450.f, 30.f});
        potionIcon2->setScale({POTION_ICON_SCALE, POTION_ICON_SCALE});

        potionIcon3 = new sf::Sprite(rm.getTexture(TextureType::Potion3));
        potionIcon3->setPosition({535.f, 30.f});
        potionIcon3->setScale({POTION_ICON_SCALE, POTION_ICON_SCALE});

        cubeIcon = new sf::Sprite(rm.getTexture(TextureType::Cube));
        cubeIcon->setPosition({620.f, 30.f});
        cubeIcon->setScale({POTION_ICON_SCALE, POTION_ICON_SCALE});

        discardPileIcon = new sf::Sprite(rm.getTexture(TextureType::DiscardPile));
        discardPileIcon->setPosition({1515.f, 25.f});
        discardPileIcon->setScale({0.45f, 0.45f});

        dialogBox = new sf::Sprite(rm.getTexture(TextureType::DialogBox));
        dialogBox->setPosition({260.f, 770.f});
        dialogBox->setScale({1.0f, 1.0f});

        hpBack.setSize({205.f, 18.f});
        hpBack.setPosition({83.f, 63.f});
        hpBack.setFillColor(sf::Color(55, 24, 24, 210));

        hpBar.setSize({205.f, 18.f});
        hpBar.setPosition({83.f, 63.f});
        hpBar.setFillColor(sf::Color(190, 42, 42, 235));

        if (hasFont) {
            hpText = new sf::Text(font);
            hpText->setString("HP 100 / 100");
            hpText->setCharacterSize(22);
            hpText->setFillColor(sf::Color::White);
            hpText->setPosition({92.f, 35.f});

            dialogText = new sf::Text(font);
            dialogText->setCharacterSize(30);
            dialogText->setFillColor(sf::Color::White);
            dialogText->setPosition({330.f, 815.f});
            dialogText->setLineSpacing(1.2f);

            dialogHintText = new sf::Text(font);
            dialogHintText->setString("Space / Enter / Click");
            dialogHintText->setCharacterSize(22);
            dialogHintText->setFillColor(sf::Color(230, 230, 230, 210));
            dialogHintText->setPosition({1335.f, 940.f});

            movementHintText = new sf::Text(font);
            movementHintText->setString("Use A / D or Arrow Keys to move");
            movementHintText->setCharacterSize(30);
            movementHintText->setFillColor(sf::Color::White);
            movementHintText->setPosition({720.f, 930.f});
        }

        DisablePotionIcons();
        settingPanel.Init(rm, &font);
        backpackPanel.Init(rm, &font);
        return true;
    } catch (const std::exception& e) {
        std::cout << "Failed to initialize UI: " << e.what() << std::endl;
        return false;
    }
}

bool UIManager::LoadFont()
{
    const std::array<std::string, 5> candidates = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/simsun.ttc",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };

    for (const auto& path : candidates) {
        if (font.openFromFile(path)) {
            return true;
        }
    }

    std::cout << "Failed to load system font. Text UI will be hidden.\n";
    return false;
}

void UIManager::SetHP(int current, int max)
{
    currentHP = std::clamp(current, 0, std::max(1, max));
    maxHP = std::max(1, max);

    const float barWidth = 205.f * (static_cast<float>(currentHP) / static_cast<float>(maxHP));
    hpBar.setSize({barWidth, 18.f});

    if (hasFont && hpText) {
        hpText->setString("HP " + std::to_string(currentHP) + " / " + std::to_string(maxHP));
    }
}

void UIManager::TakeDamage(int damage)
{
    SetHP(currentHP - damage, maxHP);
}

void UIManager::Heal(int amount)
{
    SetHP(currentHP + amount, maxHP);
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

void UIManager::ApplyDisabledIconColor(sf::Sprite* sprite){
    if (sprite) {
        sprite->setColor(sf::Color(
            static_cast<std::uint8_t>(POTION_ICON_GRAY),
            static_cast<std::uint8_t>(POTION_ICON_GRAY),
            static_cast<std::uint8_t>(POTION_ICON_GRAY),
            static_cast<std::uint8_t>(POTION_ICON_ALPHA)));
    }
}

void UIManager::DisablePotionIcons(){
    ApplyDisabledIconColor(potionIcon1);
    ApplyDisabledIconColor(potionIcon2);
    ApplyDisabledIconColor(potionIcon3);
    ApplyDisabledIconColor(cubeIcon);
}

void UIManager::EnablePotionIcon(int index){
    sf::Sprite* target = nullptr;
    if (index == 0) {
        target = potionIcon1;
    } else if (index == 1) {
        target = potionIcon2;
    } else if (index == 2) {
        target = potionIcon3;
    } else if (index == 3) {
        target = cubeIcon;
    }

    if (target) {
        target->setColor(sf::Color::White);
    }
}

void UIManager::Draw(sf::RenderWindow& window){
    if (statusBox) {
        window.draw(*statusBox);
    }
    window.draw(hpBack);
    window.draw(hpBar);
    if (hasFont && hpText) {
        window.draw(*hpText);
    }
    if (potionIcon1) {
        window.draw(*potionIcon1);
    }
    if (potionIcon2) {
        window.draw(*potionIcon2);
    }
    if (potionIcon3) {
        window.draw(*potionIcon3);
    }
    if (cubeIcon) {
        window.draw(*cubeIcon);
    }
    if (discardPileIcon) {
        window.draw(*discardPileIcon);
    }
}

void UIManager::DrawDialog(sf::RenderWindow& window, const DialogManager& dialogMgr)
{
    if (!dialogMgr.IsActive()) {
        return;
    }

    if (dialogBox) {
        window.draw(*dialogBox);
    }

    if (hasFont && dialogText && dialogHintText) {
        dialogText->setString(dialogMgr.GetCurrentText());
        window.draw(*dialogText);
        window.draw(*dialogHintText);
    }
}

void UIManager::DrawMovementHint(sf::RenderWindow& window)
{
    if (hasFont && movementHintText) {
        window.draw(*movementHintText);
    }
}

void UIManager::DrawCard(sf::RenderWindow& window, const CardView& card, float alpha){
    if (!resourceManager) {
        return;
    }

    sf::Sprite sprite(resourceManager->getTexture(card.texType));
    const auto size = sprite.getTexture().getSize();
    sprite.setOrigin({static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y) * 0.5f});
    sprite.setPosition(card.basePosition);
    sprite.setScale({0.58f, 0.58f});
    sprite.setRotation(sf::degrees(card.rotation));
    sprite.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(std::clamp(alpha, 0.f, 255.f))));
    window.draw(sprite);
}

void UIManager::DrawCardRewards(sf::RenderWindow& window, const std::vector<CardType>& cardsT, float alpha) { 
    if (alpha <= 0.f) { return; } 
    int k1 = 0, k2 = 0;
    for (int i = 0; i < cardsT.size(); i++) {
        CardView cv;
        cv.texType = cardTexMap.at(cardsT[i]);
        if(cardsT[i] == CardType::Strike6){
            cv.basePosition = {890.f + static_cast<float>(k1) * 72.f, 405.f + static_cast<float>(k1) * 5.f};
            cv.rotation = -10.f + static_cast<float>(k1) * 3.f;
            k1++;
            DrawCard(window, cv, alpha); 
        }
    }
    for (int i = 0; i < cardsT.size(); i++) {
        CardView cv;
        cv.texType = cardTexMap.at(cardsT[i]);
        if(cardsT[i] == CardType::Defend5){
            cv.basePosition = {970.f + static_cast<float>(k2) * 72.f, 505.f + static_cast<float>(k2) * 5.f},
            cv.rotation = 5.f + static_cast<float>(k2) * 3.f;
            k2++;
            DrawCard(window, cv, alpha); 
        }
    }
    if(k1 + k2 < cardsT.size()){
        std::cout<<"UIMgr : has undefined draw card rewards type"<<std::endl;
    }
}

