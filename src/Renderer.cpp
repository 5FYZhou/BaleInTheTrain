#include "Renderer.h"
#include "Scene.h"
#include <iomanip>
#include <memory>

Renderer::Renderer(ResourceManager& r, UIManager& ui) 
    : rm(r), uiManager(ui) {
}

Renderer::~Renderer() {
    delete menuBackground;
    delete gameBackground;
    delete title;
    delete startButton;
    delete settingsButton;
    delete exitButton;
}

sf::Sprite Renderer::makeSprite(const sf::Texture& texture, sf::Vector2f position) {
    sf::Sprite sprite(texture);
    sprite.setPosition(position);
    return sprite;
}

void Renderer::scaleToWindow(sf::Sprite& sprite) {
    const sf::Texture& tex = sprite.getTexture();
    const auto size = tex.getSize();
    sf::Vector2f scale(
        static_cast<float>(windowWidth) / static_cast<float>(size.x),
        static_cast<float>(windowHeight) / static_cast<float>(size.y)
    );
    sprite.setScale(scale);
}

Button Renderer::makeCenteredButton(const sf::Texture& texture, float centerY) {
    sf::Sprite sprite(texture);
    const auto size = texture.getSize();
    sprite.setPosition({
        (static_cast<float>(windowWidth) - static_cast<float>(size.x)) * 0.5f,
        centerY - static_cast<float>(size.y) * 0.5f
    });

    return {sprite, sprite.getGlobalBounds()};
}

void Renderer::Init() {
    // Load and setup menu background
    auto& menuBgTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::MenuBackground));
    menuBackground = new sf::Sprite(menuBgTex);
    menuBackground->setPosition({0.f, 0.f});
    scaleToWindow(*menuBackground);

    // Load and setup game background
    auto& gameBgTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::GameBackground));
    gameBackground = new sf::Sprite(gameBgTex);
    gameBackground->setPosition({0.f, 0.f});
    scaleToWindow(*gameBackground);

    // Load and setup title
    auto& titleTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::Title));
    title = new sf::Sprite(titleTex);
    title->setPosition({580.f, 85.f});

    // Setup menu buttons
    auto& startBtnTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::StartButton));
    startButton = new Button(makeCenteredButton(startBtnTex, 590.f));

    auto& settingsBtnTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::SettingsButton));
    settingsButton = new Button(makeCenteredButton(settingsBtnTex, 745.f));

    auto& exitBtnTex = const_cast<sf::Texture&>(rm.getTexture(TextureType::ExitButton));
    exitButton = new Button(makeCenteredButton(exitBtnTex, 900.f));

    // Setup fade overlay
    fadeOverlay.setSize(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, 0));
}

void Renderer::DrawMenu(sf::RenderWindow& window) {
    window.draw(*menuBackground);
    window.draw(*title);
    window.draw(startButton->sprite);
    window.draw(settingsButton->sprite);
    window.draw(exitButton->sprite);
}

void Renderer::DrawGame(sf::RenderWindow& window, const Player& player) {
    window.draw(*gameBackground);
    
    // Draw player sprite
    // Choose texture based on player frame

    const sf::Texture& tex = rm.getTexture(TextureType::Player, player.GetTextureIndex());
    sf::Sprite p(tex);

    // scale to player height and apply facing
    const auto size = tex.getSize();
    const float scale = player.GetWalkTimer() >= 0.f ? (player.GetIsMoving() ? 1.f : 1.f) : 1.f;
    const float scaleY = player.GetHeight() / static_cast<float>(size.y);
    p.setOrigin(sf::Vector2f(static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y)));
    p.setScale(sf::Vector2f(scaleY * static_cast<float>(player.GetFacing()), scaleY));
    p.setPosition(player.GetFeet());
    window.draw(p);

    // Draw UI
    uiManager.Draw(window);
}

void Renderer::DrawSettings(sf::RenderWindow& window) {
    window.draw(*menuBackground);
    
    sf::RectangleShape darkPanel(sf::Vector2f(760.f, 300.f));
    darkPanel.setPosition({580.f, 390.f});
    darkPanel.setFillColor(sf::Color(0, 0, 0, 165));
    window.draw(darkPanel);

    // Try to load font and display text
    sf::Font font;
    const std::array<std::string, 5> fontCandidates = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/consola.ttf"
    };

    bool hasFont = false;
    for (const auto& path : fontCandidates) {
        if (font.openFromFile(path)) {
            hasFont = true;
            break;
        }
    }

    if (hasFont) {
        sf::Text settingsText(font);
        settingsText.setString("Settings screen is reserved for the next stage.\nClick anywhere to return.");
        settingsText.setCharacterSize(32);
        settingsText.setFillColor(sf::Color::White);
        settingsText.setPosition({635.f, 485.f});
        window.draw(settingsText);
    }
}

void Renderer::DrawFadeOverlay(sf::RenderWindow& window, std::uint8_t alpha) {
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, alpha));
    window.draw(fadeOverlay);
}
