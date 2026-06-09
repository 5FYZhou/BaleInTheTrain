#include "UIManager.h"
#include <array>
#include <filesystem>

UIManager::UIManager() {
}

UIManager::~UIManager() {
    delete statusBox;
    delete potionIcon1;
    delete potionIcon2;
    delete potionIcon3;
    delete cubeIcon;
    delete settingsIcon;
    delete hpText;
}

bool UIManager::Init(ResourceManager& rm) {
    // Load font for HP text
    hasFont = LoadFont();

    try {
        // Initialize status box
        auto& statusBoxTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::StatusBox));
        statusBox = new sf::Sprite(statusBoxTex);
        statusBox->setPosition({24.f, 24.f});

        // Initialize potion icons
        auto& potion1Tex = const_cast<sf::Texture&>(rm.getTexture(TextureType::Potion1));
        potionIcon1 = new sf::Sprite(potion1Tex);
        potionIcon1->setPosition({365.f, 30.f});
        potionIcon1->setScale({0.72f, 0.72f});

        auto& potion2Tex = const_cast<sf::Texture&>(rm.getTexture(TextureType::Potion2));
        potionIcon2 = new sf::Sprite(potion2Tex);
        potionIcon2->setPosition({450.f, 30.f});
        potionIcon2->setScale({0.72f, 0.72f});

        auto& potion3Tex = const_cast<sf::Texture&>(rm.getTexture(TextureType::Potion3));
        potionIcon3 = new sf::Sprite(potion3Tex);
        potionIcon3->setPosition({535.f, 30.f});
        potionIcon3->setScale({0.72f, 0.72f});

        // Initialize cube icon
        auto& cubeTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::Cube));
        cubeIcon = new sf::Sprite(cubeTex);
        cubeIcon->setPosition({620.f, 30.f});
        cubeIcon->setScale({0.72f, 0.72f});

        // Initialize settings icon
        auto& settingsTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::SettingsIcon));
        settingsIcon = new sf::Sprite(settingsTex);
        settingsIcon->setPosition({1800.f, 30.f});
        settingsIcon->setScale({0.68f, 0.68f});

        // Initialize HP bar
        hpBack.setSize({205.f, 18.f});
        hpBack.setPosition({83.f, 63.f});
        hpBack.setFillColor(sf::Color(55, 24, 24, 210));

        hpBar.setSize({205.f, 18.f});
        hpBar.setPosition({83.f, 63.f});
        hpBar.setFillColor(sf::Color(190, 42, 42, 235));

        // Initialize HP text
        if (hasFont) {
            hpText = new sf::Text(font);
            hpText->setString("HP 100 / 100");
            hpText->setCharacterSize(22);
            hpText->setFillColor(sf::Color::White);
            hpText->setPosition({92.f, 35.f});
        }

        return true;
    } catch (const std::exception& e) {
        std::cout << "Failed to initialize UI: " << e.what() << std::endl;
        return false;
    }
}

bool UIManager::LoadFont() {
    const std::array<std::string, 5> candidates = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/consola.ttf"
    };

    for (const auto& path : candidates) {
        if (font.openFromFile(path)) {
            return true;
        }
    }

    std::cout << "Failed to load system font. HP text will be hidden.\n";
    return false;
}

void UIManager::SetHP(int current, int max) {
    currentHP = current;
    maxHP = max;

    // Update HP bar width
    float barWidth = 205.f * (static_cast<float>(current) / static_cast<float>(max));
    hpBar.setSize({barWidth, 18.f});

    // Update HP text
    if (hasFont && hpText) {
        hpText->setString("HP " + std::to_string(current) + " / " + std::to_string(max));
    }
}

void UIManager::TakeDamage(int damage) {
    currentHP = std::max(0, currentHP - damage);
    SetHP(currentHP, maxHP);
}

void UIManager::Heal(int amount) {
    currentHP = std::min(maxHP, currentHP + amount);
    SetHP(currentHP, maxHP);
}

void UIManager::Draw(sf::RenderWindow& window) {
    if (statusBox) window.draw(*statusBox);
    window.draw(hpBack);
    window.draw(hpBar);
    if (hasFont && hpText) {
        window.draw(*hpText);
    }
    if (potionIcon1) window.draw(*potionIcon1);
    if (potionIcon2) window.draw(*potionIcon2);
    if (potionIcon3) window.draw(*potionIcon3);
    if (cubeIcon) window.draw(*cubeIcon);
    if (settingsIcon) window.draw(*settingsIcon);
}
