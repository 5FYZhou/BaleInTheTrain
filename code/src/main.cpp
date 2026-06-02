#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <string>

namespace
{
constexpr unsigned int WindowWidth = 1920;
constexpr unsigned int WindowHeight = 1080;

enum class Scene
{
    Menu,
    Game,
    Settings
};

enum class FadeState
{
    None,
    FadeOut,
    FadeIn
};

enum class PlayerFrame
{
    Stand,
    StepLeft,
    StepRight
};

struct Button
{
    sf::Sprite sprite;
    sf::FloatRect bounds;
};

class Assets
{
public:
    bool loadTexture(const std::string& key, const std::string& fileName)
    {
        sf::Texture texture;
        const std::string path = assetPath(fileName);
        if (path.empty() || !texture.loadFromFile(path))
        {
            std::cout << "Failed to load texture: " << fileName << '\n';
            return false;
        }

        textures.emplace(key, std::move(texture));
        return true;
    }

    sf::Texture& texture(const std::string& key)
    {
        return textures.at(key);
    }

private:
    std::string assetPath(const std::string& fileName) const
    {
        const std::array<std::string, 5> candidates = {
            "../assets/",
            "../../assets/",
            "assets/",
            "../bale in the train/assets/",
            "../../bale in the train/assets/"
        };

        for (const auto& baseDir : candidates)
        {
            const std::string path = baseDir + fileName;
            if (std::filesystem::exists(path))
            {
                return path;
            }
        }

        return {};
    }

    std::map<std::string, sf::Texture> textures;
};

sf::Sprite makeSprite(sf::Texture& texture, sf::Vector2f position)
{
    sf::Sprite sprite(texture);
    sprite.setPosition(position);
    return sprite;
}

void scaleToWindow(sf::Sprite& sprite)
{
    const auto size = sprite.getTexture().getSize();
    sprite.setScale({
        static_cast<float>(WindowWidth) / static_cast<float>(size.x),
        static_cast<float>(WindowHeight) / static_cast<float>(size.y)
    });
}

bool loadFont(sf::Font& font)
{
    const std::array<std::string, 5> candidates = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/consola.ttf"
    };

    for (const auto& path : candidates)
    {
        if (font.openFromFile(path))
        {
            return true;
        }
    }

    std::cout << "Failed to load system font. HP text will be hidden.\n";
    return false;
}

Button makeCenteredButton(sf::Texture& texture, float centerY)
{
    sf::Sprite sprite(texture);
    const auto size = texture.getSize();
    sprite.setPosition({
        (static_cast<float>(WindowWidth) - static_cast<float>(size.x)) * 0.5f,
        centerY - static_cast<float>(size.y) * 0.5f
    });

    return {sprite, sprite.getGlobalBounds()};
}

} // namespace

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{WindowWidth, WindowHeight}),
        "Bale In The Train",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    Assets assets;
    bool loaded = true;
    loaded &= assets.loadTexture("menuBackground", "开始界面备用.png");
    loaded &= assets.loadTexture("startButton", "开始游戏选项.png");
    loaded &= assets.loadTexture("exitButton", "退出游戏选项.png");
    loaded &= assets.loadTexture("settingsButton", "设置选项.png");
    loaded &= assets.loadTexture("gameBackground", "游戏背景备用.png");
    loaded &= assets.loadTexture("baleStand", "Balestand.png");
    loaded &= assets.loadTexture("baleLeft", "Baleleft.png");
    loaded &= assets.loadTexture("baleRight", "Baleright.png");
    loaded &= assets.loadTexture("statusBox", "状态框.png");
    loaded &= assets.loadTexture("potion1", "药剂1.png");
    loaded &= assets.loadTexture("potion2", "药剂2.png");
    loaded &= assets.loadTexture("potion3", "药剂3.png");
    loaded &= assets.loadTexture("cube", "魔方.png");
    loaded &= assets.loadTexture("settingsIcon", "设置.png");
    loaded &= assets.loadTexture("title", "标题.png");
    if (!loaded)
    {
        std::cout << "Some assets failed to load. Please check the assets folder.\n";
    }

    sf::Font font;
    const bool hasFont = loadFont(font);

    sf::Sprite menuBackground = makeSprite(assets.texture("menuBackground"), {0.f, 0.f});
    sf::Sprite gameBackground = makeSprite(assets.texture("gameBackground"), {0.f, 0.f});
    scaleToWindow(menuBackground);
    scaleToWindow(gameBackground);
    sf::Sprite title = makeSprite(assets.texture("title"), {580.f, 85.f});

    Button startButton = makeCenteredButton(assets.texture("startButton"), 590.f);
    Button settingsButton = makeCenteredButton(assets.texture("settingsButton"), 745.f);
    Button exitButton = makeCenteredButton(assets.texture("exitButton"), 900.f);

    constexpr float PlayerGroundY = 1028.f;
    constexpr float PlayerHeight = 454.f;
    constexpr float PlayerStartX = 350.f;
    constexpr float PlayerLeftEntryX = 275.f;
    constexpr float PlayerRightEntryX = 1645.f;
    constexpr float PlayerLeftExitX = 210.f;
    constexpr float PlayerRightExitX = 1710.f;
    constexpr float PlayerSpeed = 430.f;
    sf::Sprite player = makeSprite(assets.texture("baleStand"), {PlayerStartX, PlayerGroundY});
    sf::Vector2f playerFeet{PlayerStartX, PlayerGroundY};
    int playerFacing = 1;
    float walkTimer = 0.f;
    bool playerMoving = false;

    auto setPlayerFrame = [&](PlayerFrame frame)
    {
        sf::Texture* texture = &assets.texture("baleStand");
        if (frame == PlayerFrame::StepLeft)
        {
            texture = &assets.texture("baleLeft");
        }
        else if (frame == PlayerFrame::StepRight)
        {
            texture = &assets.texture("baleRight");
        }

        player.setTexture(*texture, true);
        const auto size = texture->getSize();
        const float scale = PlayerHeight / static_cast<float>(size.y);
        player.setOrigin({
            static_cast<float>(size.x) * 0.5f,
            static_cast<float>(size.y)
        });
        player.setScale({
            scale * static_cast<float>(playerFacing),
            scale
        });
        player.setPosition(playerFeet);
    };
    setPlayerFrame(PlayerFrame::Stand);

    sf::Sprite statusBox = makeSprite(assets.texture("statusBox"), {24.f, 24.f});
    sf::Sprite potionIcon1 = makeSprite(assets.texture("potion1"), {365.f, 30.f});
    potionIcon1.setScale({0.72f, 0.72f});
    sf::Sprite potionIcon2 = makeSprite(assets.texture("potion2"), {450.f, 30.f});
    potionIcon2.setScale({0.72f, 0.72f});
    sf::Sprite potionIcon3 = makeSprite(assets.texture("potion3"), {535.f, 30.f});
    potionIcon3.setScale({0.72f, 0.72f});
    sf::Sprite cubeIcon = makeSprite(assets.texture("cube"), {620.f, 30.f});
    cubeIcon.setScale({0.72f, 0.72f});
    sf::Sprite settingsIcon = makeSprite(assets.texture("settingsIcon"), {1800.f, 30.f});
    settingsIcon.setScale({0.68f, 0.68f});

    sf::RectangleShape hpBack({205.f, 18.f});
    hpBack.setPosition({83.f, 63.f});
    hpBack.setFillColor(sf::Color(55, 24, 24, 210));

    sf::RectangleShape hpBar({205.f, 18.f});
    hpBar.setPosition({83.f, 63.f});
    hpBar.setFillColor(sf::Color(190, 42, 42, 235));

    sf::Text hpText(font);
    if (hasFont)
    {
        hpText.setString("HP 100 / 100");
        hpText.setCharacterSize(22);
        hpText.setFillColor(sf::Color::White);
        hpText.setPosition({92.f, 35.f});
    }

    sf::RectangleShape fadeOverlay({static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)});
    fadeOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    Scene scene = Scene::Menu;
    Scene pendingScene = Scene::Menu;
    FadeState fadeState = FadeState::None;
    float fadeAlpha = 0.f;
    int carriageIndex = 0;
    int pendingCarriageIndex = 0;
    sf::Vector2f pendingPlayerFeet = playerFeet;
    int pendingPlayerFacing = playerFacing;

    sf::Clock frameClock;

    auto beginFadeTo = [&](Scene nextScene)
    {
        if (fadeState != FadeState::None)
        {
            return;
        }

        pendingScene = nextScene;
        pendingCarriageIndex = carriageIndex;
        pendingPlayerFeet = playerFeet;
        pendingPlayerFacing = playerFacing;
        fadeState = FadeState::FadeOut;
        fadeAlpha = 0.f;
    };

    auto beginCarriageFade = [&](int nextCarriage, float nextX, int nextFacing)
    {
        if (fadeState != FadeState::None)
        {
            return;
        }

        pendingScene = Scene::Game;
        pendingCarriageIndex = nextCarriage;
        pendingPlayerFeet = {nextX, PlayerGroundY};
        pendingPlayerFacing = nextFacing;
        fadeState = FadeState::FadeOut;
        fadeAlpha = 0.f;
    };

    auto drawMenu = [&]()
    {
        window.draw(menuBackground);
        window.draw(title);
        window.draw(startButton.sprite);
        window.draw(settingsButton.sprite);
        window.draw(exitButton.sprite);
    };

    auto drawGame = [&]()
    {
        window.draw(gameBackground);
        window.draw(player);

        window.draw(statusBox);
        window.draw(hpBack);
        window.draw(hpBar);
        if (hasFont)
        {
            window.draw(hpText);
        }
        window.draw(potionIcon1);
        window.draw(potionIcon2);
        window.draw(potionIcon3);
        window.draw(cubeIcon);
        window.draw(settingsIcon);
    };

    while (window.isOpen())
    {
        const float dt = frameClock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->scancode == sf::Keyboard::Scancode::Escape)
                {
                    if (scene == Scene::Game)
                    {
                        beginFadeTo(Scene::Menu);
                    }
                    else
                    {
                        window.close();
                    }
                }

            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button != sf::Mouse::Button::Left || fadeState != FadeState::None)
                {
                    continue;
                }

                const sf::Vector2f mousePos{
                    static_cast<float>(mouse->position.x),
                    static_cast<float>(mouse->position.y)
                };

                if (scene == Scene::Menu)
                {
                    if (startButton.bounds.contains(mousePos))
                    {
                        beginFadeTo(Scene::Game);
                    }
                    else if (settingsButton.bounds.contains(mousePos))
                    {
                        beginFadeTo(Scene::Settings);
                    }
                    else if (exitButton.bounds.contains(mousePos))
                    {
                        window.close();
                    }
                }
                else if (scene == Scene::Settings)
                {
                    beginFadeTo(Scene::Menu);
                }
            }
        }

        playerMoving = false;
        if (scene == Scene::Game && fadeState == FadeState::None)
        {
            int movementDirection = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
            {
                movementDirection -= 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
            {
                movementDirection += 1;
            }

            if (movementDirection != 0)
            {
                playerMoving = true;
                playerFacing = movementDirection;
                playerFeet.x += PlayerSpeed * static_cast<float>(movementDirection) * dt;

                if (playerFeet.x <= PlayerLeftExitX)
                {
                    if (carriageIndex > 0)
                    {
                        beginCarriageFade(carriageIndex - 1, PlayerRightEntryX, -1);
                    }
                    playerFeet.x = PlayerLeftExitX;
                }
                else if (playerFeet.x >= PlayerRightExitX)
                {
                    if (carriageIndex < 3)
                    {
                        beginCarriageFade(carriageIndex + 1, PlayerLeftEntryX, 1);
                    }
                    playerFeet.x = PlayerRightExitX;
                }
            }

            playerFeet.x = std::clamp(playerFeet.x, PlayerLeftExitX, PlayerRightExitX);
            playerFeet.y = PlayerGroundY;
        }

        if (playerMoving)
        {
            walkTimer += dt;
            const bool useLeftStep = static_cast<int>(walkTimer * 8.f) % 2 == 0;
            setPlayerFrame(useLeftStep ? PlayerFrame::StepLeft : PlayerFrame::StepRight);
        }
        else
        {
            walkTimer = 0.f;
            setPlayerFrame(PlayerFrame::Stand);
        }

        if (fadeState == FadeState::FadeOut)
        {
            fadeAlpha += 520.f * dt;
            if (fadeAlpha >= 255.f)
            {
                fadeAlpha = 255.f;
                scene = pendingScene;
                carriageIndex = pendingCarriageIndex;
                playerFeet = pendingPlayerFeet;
                playerFacing = pendingPlayerFacing;
                setPlayerFrame(PlayerFrame::Stand);
                fadeState = FadeState::FadeIn;
            }
        }
        else if (fadeState == FadeState::FadeIn)
        {
            fadeAlpha -= 520.f * dt;
            if (fadeAlpha <= 0.f)
            {
                fadeAlpha = 0.f;
                fadeState = FadeState::None;
            }
        }

        fadeOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));

        window.clear();

        if (scene == Scene::Menu)
        {
            drawMenu();
        }
        else if (scene == Scene::Game)
        {
            drawGame();
        }
        else
        {
            drawMenu();
            sf::RectangleShape darkPanel({760.f, 300.f});
            darkPanel.setPosition({580.f, 390.f});
            darkPanel.setFillColor(sf::Color(0, 0, 0, 165));
            window.draw(darkPanel);

            if (hasFont)
            {
                sf::Text settingsText(font);
                settingsText.setString("Settings screen is reserved for the next stage.\nClick anywhere to return.");
                settingsText.setCharacterSize(32);
                settingsText.setFillColor(sf::Color::White);
                settingsText.setPosition({635.f, 485.f});
                window.draw(settingsText);
            }
        }

        if (fadeAlpha > 0.f)
        {
            window.draw(fadeOverlay);
        }

        window.display();
    }

    return 0;
}
