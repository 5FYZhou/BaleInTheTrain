#include "Renderer.h"
#include "Scene/Scene.h"
#include <iomanip>
#include <memory>
#include <algorithm>

Renderer::Renderer(ResourceManager &r, UIManager &ui)
    : rm(r), uiManager(ui)
{
    font = &rm.getFont();
}

Renderer::~Renderer()
{
    delete hpText;
    delete font;
    delete statusBox;

    delete dialogBox;
    delete dialogText;
    delete dialogHintText;
    delete movementHintText;
}

sf::Sprite Renderer::makeSprite(const sf::Texture &texture, sf::Vector2f position)
{
    sf::Sprite sprite(texture);
    sprite.setPosition(position);
    return sprite;
}

/*
Button Renderer::makeCenteredButton(const sf::Texture& texture, float centerY) {
    sf::Sprite sprite(texture);
    const auto size = texture.getSize();
    sprite.setPosition({
        (static_cast<float>(windowWidth) - static_cast<float>(size.x)) * 0.5f,
        centerY - static_cast<float>(size.y) * 0.5f
    });

    return {sprite, sprite.getGlobalBounds()};
}*/

void Renderer::Init()
{
    // Setup fade overlay
    fadeOverlay.setSize(sf::Vector2f(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)));
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    // player HP
    statusBox = new sf::Sprite(rm.getTexture(TextureType::StatusBox));
    statusBox->setPosition({24.f, 24.f});
    hpBack.setSize({205.f, 18.f});
    hpBack.setPosition({83.f, 63.f});
    hpBack.setFillColor(sf::Color(55, 24, 24, 210));

    hpBar.setSize({205.f, 18.f});
    hpBar.setPosition({83.f, 63.f});
    hpBar.setFillColor(sf::Color(190, 42, 42, 235));

    hpText = new sf::Text(*font);
    hpText->setString("HP 100 / 100");
    hpText->setCharacterSize(22);
    hpText->setFillColor(sf::Color::White);
    hpText->setPosition({92.f, 35.f});

    dialogBox = new sf::Sprite(rm.getTexture(TextureType::DialogBox));
    dialogBox->setPosition({260.f, 870.f});
    dialogBox->setScale({1.0f, 1.0f});
    centerSpriteX(*dialogBox);

    dialogText = new sf::Text(*font);
    dialogText->setCharacterSize(30);
    dialogText->setFillColor(sf::Color::White);
    dialogText->setPosition({330.f, 815.f});
    dialogText->setLineSpacing(1.2f);

    dialogHintText = new sf::Text(*font);
    dialogHintText->setString(utf8("点击鼠标左键 / 空格 / 回车 继续"));
    dialogHintText->setCharacterSize(22);
    dialogHintText->setFillColor(sf::Color(230, 230, 230, 210));
    dialogHintText->setPosition({1335.f, 920.f});

    movementHintText = new sf::Text(*font);
    movementHintText->setString(utf8("按 A / D 或方向键左右移动"));
    movementHintText->setCharacterSize(30);
    movementHintText->setFillColor(sf::Color::White);
    movementHintText->setPosition({720.f, 930.f});
}

void Renderer::DrawPlayer(sf::RenderWindow &window, const Player &player)
{
    // 获取对应frame编号的纹理
    const sf::Texture &tex = rm.getTexture(TextureType::Player, player.GetTextureIndex());
    sf::Sprite p(tex);

    // scale to player height and apply facing
    const auto size = tex.getSize();
    const float scale = player.GetWalkTimer() >= 0.f ? (player.GetIsMoving() ? 1.f : 1.f) : 1.f;
    const float scaleY = 1.2f; // player.GetHeight() / static_cast<float>(size.y);
    p.setOrigin(sf::Vector2f(static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y)));
    p.setScale(sf::Vector2f(scaleY * static_cast<float>(-player.GetFacing()), scaleY));
    p.setPosition(player.GetFeet());
    window.draw(p);

    // player HP
    int currentHP = player.GetCurrentHP();
    int maxHP = player.GetMaxHP();
    const float barWidth = 205.f * (static_cast<float>(currentHP) / static_cast<float>(maxHP));
    hpBar.setSize({barWidth, 18.f});
    if (/*hasFont &&*/ hpText)
    {
        hpText->setString("HP " + std::to_string(currentHP) + " / " + std::to_string(maxHP));
    }
    window.draw(*statusBox);
    window.draw(hpBar);
    window.draw(*hpText);
}

void Renderer::DrawScene(sf::RenderWindow &window, Scene &scene)
{
    // 背景
    sf::Sprite bg(rm.getTexture(scene.GetBgTextrue()));
    scaleToWindow(bg);
    window.draw(bg);
    // 交互物品
    for (const auto &item : scene.GetInteractables())
    {
        // DrawItem(window, item.position, item.texture, item.scale);
        sf::Sprite sprite(rm.getTexture(item.texture));
        sprite.setPosition(item.position);
        sprite.setScale(item.scale);
        window.draw(sprite);
    }
    // 敌人
    auto ev = scene.GetEnemyV();
    if (!ev)
        return;
    for (auto &e : *ev)
    {
        if (!e.dead)
        {
            sf::Sprite es(rm.getTexture(enemyTexMap.at(e.ty), e.frameIndex));
            es.setPosition(e.position);
            es.setScale(e.scale);
            window.draw(es);
        }
    }
    // 血条
    if (scene.GetType() == SceneType::Battle)
    {
        auto e = scene.GetClickEnemy();
        if (e && !e->dead)
        {

            constexpr float BAR_WIDTH = 120.f;
            constexpr float BAR_HEIGHT = 12.f;

            float hpPercent =
                static_cast<float>(e->cur_health) /
                static_cast<float>(e->sum_health);

            hpPercent = std::clamp(hpPercent, 0.f, 1.f);

            auto bounds = e->bound;

            float x = bounds.position.x + (bounds.size.x - BAR_WIDTH) * 0.5f;
            float y = e->position.y + e->HPDrawOffset; // SFML 3

            // 黑色背景
            sf::RectangleShape bgBar({BAR_WIDTH, BAR_HEIGHT});
            bgBar.setPosition({x, y});
            bgBar.setFillColor(sf::Color::Black);

            // 红色血量
            sf::RectangleShape hpBar({BAR_WIDTH * hpPercent, BAR_HEIGHT});
            hpBar.setPosition({x, y});
            hpBar.setFillColor(sf::Color::Red);

            // 可选：白色边框
            sf::RectangleShape border({BAR_WIDTH, BAR_HEIGHT});
            border.setPosition({x, y});
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(0.5f);
            border.setOutlineColor(sf::Color::White);

            window.draw(bgBar);
            window.draw(hpBar);
            window.draw(border);

            // HP文字
            sf::Text hpText(*font);
            hpText.setString(
                std::to_string(e->cur_health) +
                "/" +
                std::to_string(e->sum_health));

            hpText.setCharacterSize(20);
            hpText.setFillColor(sf::Color::White);

            // 黑色描边提高可读性
            hpText.setOutlineColor(sf::Color::Black);
            hpText.setOutlineThickness(1.f);

            CenterOrigin(hpText);

            hpText.setPosition({x + BAR_WIDTH * 0.5f,
                                y + BAR_HEIGHT * 0.5f});

            window.draw(hpText);
        }
    }
}

void Renderer::DrawFadeOverlay(sf::RenderWindow &window, std::uint8_t alpha)
{
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, alpha));
    window.draw(fadeOverlay);
}

void Renderer::DrawItem(sf::RenderWindow &window, sf::Vector2f position, const TextureType &type, sf::Vector2f scale, int index)
{
    sf::Sprite sprite(rm.getTexture(type, index));
    sprite.setPosition(position);
    sprite.setScale(scale);
    window.draw(sprite);
}

void Renderer::DrawItemWithNum(sf::RenderWindow &window, TextureType type, int num, sf::Vector2f pos)
{
    sf::Sprite sp(rm.getTexture(type));
    CenterOrigin(sp);
    sp.setPosition(pos);
    FitSprite(sp, {65, 65});
    window.draw(sp);

    pos.x += 50;
    sf::Text t(*font);
    t.setString(std::to_string(num));
    CenterOrigin(t);
    t.setPosition(pos);
    t.setFillColor(sf::Color::White);
    t.setCharacterSize(30);
    window.draw(t);
}

void Renderer::DrawDialog(sf::RenderWindow &window, const TextHintManager &textHintMgr)
{
    if (!textHintMgr.IsActive())
    {
        return;
    }

    if (dialogBox)
    {
        window.draw(*dialogBox);
    }

    if (dialogText && dialogHintText)
    {
        dialogText->setString(utf8(textHintMgr.GetCurrentText()));
        window.draw(*dialogText);
        window.draw(*dialogHintText);
    }
}

void Renderer::DrawMovementHint(sf::RenderWindow &window)
{
    if (movementHintText)
    {
        window.draw(*movementHintText);
    }
}

void Renderer::DrawCard(sf::RenderWindow &window, const CardView &card, float alpha)
{
    sf::Sprite sprite(rm.getTexture(card.texType));

    const auto size = sprite.getTexture().getSize();
    sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});

    sprite.setPosition(card.basePosition);

    float scale = (card.scale > 0.f) ? card.scale : 0.58f;
    sprite.setScale({scale, scale});

    sprite.setRotation(sf::degrees(card.rotation));

    sf::Color c = sprite.getColor();
    c.a = static_cast<std::uint8_t>(std::clamp(alpha, 0.f, 255.f));
    sprite.setColor(c);

    window.draw(sprite);
}

void Renderer::DrawCenteredText(sf::RenderWindow &window, const std::string &text, float alpha)
{
    sf::Text t(*font);
    t.setString(utf8(text));
    t.setCharacterSize(40);

    t.setFillColor(sf::Color(255, 255, 255,
                             static_cast<std::uint8_t>(alpha)));

    sf::FloatRect bounds = t.getLocalBounds();

    t.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                 bounds.position.y + bounds.size.y / 2.f});

    t.setPosition({window.getSize().x / 2.f,
                   window.getSize().y / 2.f});

    window.draw(t);
}

void Renderer::DrawText(
    sf::RenderWindow &window,
    const std::string &text,
    sf::Vector2f position,
    unsigned size,
    float alpha)
{
    sf::Text sfText(*font);

    sfText.setString(utf8(text));
    sfText.setCharacterSize(size);

    auto bounds = sfText.getLocalBounds();

    sfText.setOrigin({bounds.position.x + bounds.size.x * 0.5f,
                      bounds.position.y + bounds.size.y * 0.5f});

    sfText.setPosition(position);

    auto color = sfText.getFillColor();
    color.a = static_cast<std::uint8_t>(
        std::clamp(alpha, 0.f, 255.f));

    sfText.setFillColor(color);

    window.draw(sfText);
}
