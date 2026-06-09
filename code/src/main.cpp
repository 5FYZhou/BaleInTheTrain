#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace
{
constexpr unsigned int WindowWidth = 1920;
constexpr unsigned int WindowHeight = 1080;

enum class Scene { Menu, Game, Backpack };
enum class FadeState { None, FadeOut, FadeIn };
enum class SliderTarget { None, Music, Sfx };

struct Button
{
    sf::Sprite sprite;
    sf::FloatRect bounds;
};

struct CardView
{
    std::string textureKey;
    sf::Vector2f basePosition;
    float rotation;
};

sf::String utf8(const std::string& text)
{
    return sf::String::fromUtf8(text.begin(), text.end());
}

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
            "../assets/", "../../assets/", "assets/",
            "../bale in the train/assets/", "../../bale in the train/assets/"
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
        "C:/Windows/Fonts/simsun.ttc",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };

    for (const auto& path : candidates)
    {
        if (font.openFromFile(path))
        {
            return true;
        }
    }

    std::cout << "Failed to load system font. Text will be hidden.\n";
    return false;
}

Button makeCenteredButton(sf::Texture& texture, float centerY, float scale)
{
    sf::Sprite sprite(texture);
    const auto size = texture.getSize();
    sprite.setScale({scale, scale});
    sprite.setPosition({
        (static_cast<float>(WindowWidth) - static_cast<float>(size.x) * scale) * 0.5f,
        centerY - static_cast<float>(size.y) * scale * 0.5f
    });

    return {sprite, sprite.getGlobalBounds()};
}

bool pointInRect(sf::Vector2f point, sf::FloatRect rect)
{
    return rect.contains(point);
}

std::string audioPath(const std::string& fileName)
{
    const std::array<std::string, 5> candidates = {
        "../audio/", "../../audio/", "audio/",
        "../bale in the train/audio/", "../../bale in the train/audio/"
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
    loaded &= assets.loadTexture("backpackInterior", "背包内部.png");
    loaded &= assets.loadTexture("settingsPanel", "设置面板.png");
    loaded &= assets.loadTexture("closeButton", "叉叉键.png");
    loaded &= assets.loadTexture("backButton", "返回键.png");
    loaded &= assets.loadTexture("baleStand", "balestand1.png");
    loaded &= assets.loadTexture("baleWalk1", "bale1.png");
    loaded &= assets.loadTexture("baleWalk2", "bale2.png");
    loaded &= assets.loadTexture("baleWalk3", "bale3.png");
    loaded &= assets.loadTexture("baleWalk4", "bale4.png");
    loaded &= assets.loadTexture("statusBox", "状态框.png");
    loaded &= assets.loadTexture("potion1", "药剂1.png");
    loaded &= assets.loadTexture("potion2", "药剂2.png");
    loaded &= assets.loadTexture("potion3", "药剂3.png");
    loaded &= assets.loadTexture("cube", "魔方.png");
    loaded &= assets.loadTexture("backpack", "背包.png");
    loaded &= assets.loadTexture("discardPile", "弃牌池.png");
    loaded &= assets.loadTexture("strike6", "打击6点.png");
    loaded &= assets.loadTexture("defend5", "防御5点.png");
    loaded &= assets.loadTexture("dialogBox", "对话框.png");
    loaded &= assets.loadTexture("settingsIcon", "设置.png");
    loaded &= assets.loadTexture("title", "标题.png");
    if (!loaded)
    {
        std::cout << "Some assets failed to load. Please check the assets folder.\n";
    }

    sf::Font font;
    const bool hasFont = loadFont(font);

    auto loadSoundBuffer = [](sf::SoundBuffer& buffer, const std::string& fileName)
    {
        const std::string path = audioPath(fileName);
        if (path.empty() || !buffer.loadFromFile(path))
        {
            std::cout << "Failed to load sound: " << fileName << '\n';
            return false;
        }

        return true;
    };

    sf::SoundBuffer dialogBuffer;
    sf::SoundBuffer tutorialBuffer;
    sf::SoundBuffer footstepBuffer;
    sf::SoundBuffer backBuffer;
    sf::SoundBuffer menuButtonBuffer;
    loadSoundBuffer(dialogBuffer, "对话音效.mp3");
    loadSoundBuffer(tutorialBuffer, "教学音效.mp3");
    loadSoundBuffer(footstepBuffer, "走路音效.mp3");
    loadSoundBuffer(backBuffer, "返回键音效.mp3");
    loadSoundBuffer(menuButtonBuffer, "开始界面按钮音效.wav");

    sf::Sound dialogSound(dialogBuffer);
    sf::Sound tutorialSound(tutorialBuffer);
    sf::Sound footstepSound(footstepBuffer);
    sf::Sound backSound(backBuffer);
    sf::Sound menuButtonSound(menuButtonBuffer);

    sf::Sprite menuBackground = makeSprite(assets.texture("menuBackground"), {0.f, 0.f});
    sf::Sprite gameBackground = makeSprite(assets.texture("gameBackground"), {0.f, 0.f});
    sf::Sprite backpackInterior = makeSprite(assets.texture("backpackInterior"), {0.f, 0.f});
    scaleToWindow(menuBackground);
    scaleToWindow(gameBackground);
    scaleToWindow(backpackInterior);

    sf::Sprite title = makeSprite(assets.texture("title"), {580.f, 85.f});
    Button startButton = makeCenteredButton(assets.texture("startButton"), 630.f, 0.85f);
    Button settingsButton = makeCenteredButton(assets.texture("settingsButton"), 760.f, 0.85f);
    Button exitButton = makeCenteredButton(assets.texture("exitButton"), 890.f, 0.85f);

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

    auto setPlayerTexture = [&](const std::string& textureKey)
    {
        sf::Texture& texture = assets.texture(textureKey);
        player.setTexture(texture, true);
        const auto size = texture.getSize();
        const float scale = PlayerHeight / static_cast<float>(size.y);
        player.setOrigin({static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y)});
        player.setScale({-scale * static_cast<float>(playerFacing), scale});
        player.setPosition(playerFeet);
    };
    setPlayerTexture("baleStand");

    sf::Sprite statusBox = makeSprite(assets.texture("statusBox"), {24.f, 32.f});
    sf::Sprite potionIcon1 = makeSprite(assets.texture("potion1"), {365.f, 30.f});
    potionIcon1.setScale({0.72f, 0.72f});
    potionIcon1.setColor(sf::Color(120, 120, 120, 155));
    sf::Sprite potionIcon2 = makeSprite(assets.texture("potion2"), {450.f, 30.f});
    potionIcon2.setScale({0.72f, 0.72f});
    potionIcon2.setColor(sf::Color(120, 120, 120, 155));
    sf::Sprite potionIcon3 = makeSprite(assets.texture("potion3"), {535.f, 30.f});
    potionIcon3.setScale({0.72f, 0.72f});
    potionIcon3.setColor(sf::Color(120, 120, 120, 155));
    sf::Sprite cubeIcon = makeSprite(assets.texture("cube"), {620.f, 30.f});
    cubeIcon.setScale({0.72f, 0.72f});
    cubeIcon.setColor(sf::Color(120, 120, 120, 155));

    sf::Sprite backpackIcon = makeSprite(assets.texture("backpack"), {1695.f, 25.f});
    backpackIcon.setScale({0.45f, 0.45f});
    sf::Sprite settingsIcon = makeSprite(assets.texture("settingsIcon"), {1800.f, 30.f});
    settingsIcon.setScale({0.68f, 0.68f});
    sf::Sprite discardPileIcon = makeSprite(assets.texture("discardPile"), {1770.f, 885.f});
    discardPileIcon.setScale({0.72f, 0.72f});

    sf::RectangleShape hpBack({205.f, 18.f});
    hpBack.setPosition({83.f, 75.f});
    hpBack.setFillColor(sf::Color(55, 24, 24, 210));
    sf::RectangleShape hpBar({205.f, 18.f});
    hpBar.setPosition({83.f, 75.f});
    hpBar.setFillColor(sf::Color(190, 42, 42, 235));

    sf::Text hpText(font);
    if (hasFont)
    {
        hpText.setString("HP 100 / 100");
        hpText.setCharacterSize(22);
        hpText.setFillColor(sf::Color::White);
        hpText.setPosition({92.f, 45.f});
    }

    sf::Sprite dialogBox = makeSprite(assets.texture("dialogBox"), {160.f, 860.f});
    std::vector<std::string> introDialog = {
        "Bale:\n……这是哪？刚才那阵撞击之后，整节车厢都安静得不正常。",
        "Bale:\n怎么没有人？大家都去哪里了？车子停下来了，试试打开门。",
        "Bale:\n门被锁住了。想离开这里，必须找到钥匙，或者能拼成钥匙的碎片。",
        "Bale:\n这里有个背包。里面放着5张“打击6点”和5张“防御5点”的卡牌。",
        "Bale:\n这些牌也许能在战斗里派上用场。先收好。接下来，去别的车厢看看。"
    };
    constexpr std::size_t BackpackIntroIndex = 3;
    bool introActive = false;
    bool movementHintVisible = false;
    bool introRewardFading = false;
    bool settingsPopupOpen = false;
    float movementHintTimer = 0.f;
    float introRewardAlpha = 0.f;
    std::size_t introDialogIndex = 0;

    sf::Text dialogText(font);
    sf::Text dialogHintText(font);
    sf::Text movementHintText(font);
    if (hasFont)
    {
        dialogText.setCharacterSize(32);
        dialogText.setFillColor(sf::Color::White);
        dialogText.setPosition({255.f, 910.f});
        dialogHintText.setString(utf8("点击鼠标左键 / 空格 / 回车 继续"));
        dialogHintText.setCharacterSize(22);
        dialogHintText.setFillColor(sf::Color(210, 210, 210, 230));
        dialogHintText.setPosition({1280.f, 1018.f});
        movementHintText.setString(utf8("按 A / D 或方向键左右移动"));
        movementHintText.setCharacterSize(34);
        movementHintText.setFillColor(sf::Color::White);
    }

    std::vector<CardView> backpackCardViews;
    for (int i = 0; i < 5; ++i)
    {
        backpackCardViews.push_back({"strike6", {410.f + i * 128.f, 525.f + i * 7.f}, -7.f + i * 1.5f});
    }
    for (int i = 0; i < 5; ++i)
    {
        backpackCardViews.push_back({"defend5", {1010.f + i * 128.f, 525.f + i * 7.f}, -7.f + i * 1.5f});
    }

    sf::Sprite backpackBackButton = makeSprite(assets.texture("backButton"), {50.f, 55.f});
    backpackBackButton.setScale({0.7f, 0.7f});

    sf::Sprite rewardBackpack = makeSprite(assets.texture("backpack"), {760.f, 405.f});
    rewardBackpack.setScale({0.8f, 0.8f});
    std::vector<CardView> rewardCards = {
        {"strike6", {890.f, 405.f}, -10.f},
        {"strike6", {930.f, 425.f}, -5.f},
        {"strike6", {970.f, 445.f}, 0.f},
        {"defend5", {1010.f, 425.f}, 5.f},
        {"defend5", {1050.f, 405.f}, 10.f}
    };

    sf::Sprite settingsPanel = makeSprite(assets.texture("settingsPanel"), {628.f, 315.f});
    settingsPanel.setScale({1.f, 1.f});
    sf::Sprite closeSettingsButton = makeSprite(assets.texture("closeButton"), {1280.f, 280.f});
    closeSettingsButton.setScale({0.7f, 0.7f});

    float musicVolume = 0.7f;
    float sfxVolume = 0.75f;
    SliderTarget draggingSlider = SliderTarget::None;

    auto playSfx = [&](sf::Sound& sound, float volumeScale = 1.f)
    {
        sound.setVolume(100.f * sfxVolume * volumeScale);
        sound.play();
    };

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

    auto beginNewGameFade = [&]()
    {
        if (fadeState != FadeState::None)
        {
            return;
        }

        pendingScene = Scene::Game;
        pendingCarriageIndex = 0;
        pendingPlayerFeet = {PlayerStartX, PlayerGroundY};
        pendingPlayerFacing = 1;
        introActive = true;
        movementHintVisible = false;
        introRewardFading = false;
        settingsPopupOpen = false;
        introRewardAlpha = 0.f;
        introDialogIndex = 0;
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

    auto drawCard = [&](const CardView& card, sf::Vector2f mousePos, bool interactive)
    {
        sf::Sprite sprite(assets.texture(card.textureKey));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y) * 0.5f});
        sprite.setPosition(card.basePosition);
        sprite.setScale({0.58f, 0.58f});
        sprite.setRotation(sf::degrees(card.rotation));

        const bool hovered = interactive && pointInRect(mousePos, sprite.getGlobalBounds());
        if (hovered)
        {
            sprite.setPosition({card.basePosition.x, card.basePosition.y - 44.f});
            sprite.setScale({0.64f, 0.64f});
        }

        window.draw(sprite);
    };

    auto drawRewardCards = [&](float alpha)
    {
        sf::Sprite bag = rewardBackpack;
        bag.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
        window.draw(bag);

        for (const auto& card : rewardCards)
        {
            sf::Sprite sprite(assets.texture(card.textureKey));
            const auto size = sprite.getTexture().getSize();
            sprite.setOrigin({static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y) * 0.5f});
            sprite.setPosition(card.basePosition);
            sprite.setScale({0.42f, 0.42f});
            sprite.setRotation(sf::degrees(card.rotation));
            sprite.setColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
            window.draw(sprite);
        }
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
        window.draw(backpackIcon);
        window.draw(settingsIcon);
    };

    auto drawBackpackScene = [&](sf::Vector2f mousePos)
    {
        window.draw(backpackInterior);
        sf::RectangleShape veil({static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)});
        veil.setFillColor(sf::Color(0, 0, 0, 55));
        window.draw(veil);
        window.draw(backpackBackButton);

        for (const auto& card : backpackCardViews)
        {
            drawCard(card, mousePos, true);
        }

        if (hasFont)
        {
            sf::Text titleText(font);
            titleText.setString(utf8("背包"));
            titleText.setCharacterSize(50);
            titleText.setFillColor(sf::Color::White);
            titleText.setPosition({145.f, 62.f});
            window.draw(titleText);
        }
    };

    auto sliderValueFromX = [](float x, float left, float width)
    {
        return std::clamp((x - left) / width, 0.f, 1.f);
    };

    auto drawSlider = [&](const std::string& label, float value, sf::Vector2f pos)
    {
        constexpr float SliderWidth = 360.f;
        sf::RectangleShape track({SliderWidth, 10.f});
        track.setPosition({pos.x, pos.y + 18.f});
        track.setFillColor(sf::Color(75, 70, 68, 210));
        window.draw(track);

        sf::RectangleShape fill({SliderWidth * value, 10.f});
        fill.setPosition({pos.x, pos.y + 18.f});
        fill.setFillColor(sf::Color(230, 214, 180, 240));
        window.draw(fill);

        sf::CircleShape knob(18.f);
        knob.setPosition({pos.x + SliderWidth * value - 18.f, pos.y});
        knob.setFillColor(sf::Color::White);
        window.draw(knob);

        if (hasFont)
        {
            sf::Text labelText(font);
            labelText.setString(utf8(label + "  " + std::to_string(static_cast<int>(value * 100.f)) + "%"));
            labelText.setCharacterSize(25);
            labelText.setFillColor(sf::Color::White);
            labelText.setPosition({pos.x, pos.y - 42.f});
            window.draw(labelText);
        }
    };

    auto drawSettingsPopup = [&]()
    {
        sf::RectangleShape veil({static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)});
        veil.setFillColor(sf::Color(0, 0, 0, 95));
        window.draw(veil);
        window.draw(settingsPanel);
        window.draw(closeSettingsButton);

        if (hasFont)
        {
            sf::Text titleText(font);
            titleText.setString(utf8("设置"));
            titleText.setCharacterSize(42);
            titleText.setFillColor(sf::Color::White);
            titleText.setPosition({700.f, 365.f});
            window.draw(titleText);
        }

        drawSlider("音乐声音大小", musicVolume, {760.f, 500.f});
        drawSlider("音效声音大小", sfxVolume, {760.f, 610.f});
    };

    auto advanceIntroDialog = [&]()
    {
        if (!introActive)
        {
            return;
        }

        playSfx(dialogSound);

        if (introDialogIndex == BackpackIntroIndex)
        {
            introRewardFading = true;
            introRewardAlpha = 255.f;
        }

        if (introDialogIndex + 1 < introDialog.size())
        {
            ++introDialogIndex;
        }
        else
        {
            introActive = false;
            movementHintVisible = true;
            movementHintTimer = 0.f;
            playSfx(tutorialSound);
        }
    };

    while (window.isOpen())
    {
        const float dt = frameClock.restart().asSeconds();
        const auto mousePixel = sf::Mouse::getPosition(window);
        const sf::Vector2f mousePos{static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y)};

        if (draggingSlider == SliderTarget::Music)
        {
            musicVolume = sliderValueFromX(mousePos.x, 760.f, 360.f);
        }
        else if (draggingSlider == SliderTarget::Sfx)
        {
            sfxVolume = sliderValueFromX(mousePos.x, 760.f, 360.f);
        }

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (scene == Scene::Game && introActive &&
                    (key->scancode == sf::Keyboard::Scancode::Space ||
                     key->scancode == sf::Keyboard::Scancode::Enter))
                {
                    advanceIntroDialog();
                    continue;
                }

                if (key->scancode == sf::Keyboard::Scancode::Escape)
                {
                    if (settingsPopupOpen)
                    {
                        settingsPopupOpen = false;
                    }
                    else if (scene == Scene::Backpack)
                    {
                        beginFadeTo(Scene::Game);
                    }
                    else if (scene == Scene::Game)
                    {
                        beginFadeTo(Scene::Menu);
                    }
                    else
                    {
                        window.close();
                    }
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>())
            {
                draggingSlider = SliderTarget::None;
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button != sf::Mouse::Button::Left || fadeState != FadeState::None)
                {
                    continue;
                }

                const sf::Vector2f clickPos{
                    static_cast<float>(mouse->position.x),
                    static_cast<float>(mouse->position.y)
                };

                if (settingsPopupOpen)
                {
                    if (pointInRect(clickPos, closeSettingsButton.getGlobalBounds()))
                    {
                        playSfx(backSound);
                        settingsPopupOpen = false;
                        draggingSlider = SliderTarget::None;
                    }
                    else if (pointInRect(clickPos, sf::FloatRect({735.f, 482.f}, {410.f, 58.f})))
                    {
                        draggingSlider = SliderTarget::Music;
                        musicVolume = sliderValueFromX(clickPos.x, 760.f, 360.f);
                    }
                    else if (pointInRect(clickPos, sf::FloatRect({735.f, 592.f}, {410.f, 58.f})))
                    {
                        draggingSlider = SliderTarget::Sfx;
                        sfxVolume = sliderValueFromX(clickPos.x, 760.f, 360.f);
                    }
                    continue;
                }

                if (scene == Scene::Menu)
                {
                    if (startButton.bounds.contains(clickPos))
                    {
                        playSfx(menuButtonSound);
                        beginNewGameFade();
                    }
                    else if (settingsButton.bounds.contains(clickPos))
                    {
                        playSfx(menuButtonSound);
                    }
                    else if (exitButton.bounds.contains(clickPos))
                    {
                        playSfx(menuButtonSound);
                        window.close();
                    }
                }
                else if (scene == Scene::Game && introActive)
                {
                    advanceIntroDialog();
                }
                else if (scene == Scene::Game)
                {
                    if (pointInRect(clickPos, backpackIcon.getGlobalBounds()))
                    {
                        beginFadeTo(Scene::Backpack);
                    }
                    else if (pointInRect(clickPos, settingsIcon.getGlobalBounds()))
                    {
                        settingsPopupOpen = true;
                    }
                }
                else if (scene == Scene::Backpack)
                {
                    if (pointInRect(clickPos, backpackBackButton.getGlobalBounds()))
                    {
                        playSfx(backSound);
                        beginFadeTo(Scene::Game);
                    }
                }
            }
        }

        playerMoving = false;
        if (scene == Scene::Game && fadeState == FadeState::None && !introActive && !settingsPopupOpen)
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
                movementHintVisible = false;
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
            if (footstepSound.getStatus() != sf::SoundSource::Status::Playing)
            {
                playSfx(footstepSound, 0.65f);
            }
            const int frameIndex = static_cast<int>(walkTimer * 10.f) % 4;
            const std::array<std::string, 4> walkFrames = {
                "baleWalk1", "baleWalk2", "baleWalk3", "baleWalk4"
            };
            setPlayerTexture(walkFrames[frameIndex]);
        }
        else
        {
            if (footstepSound.getStatus() == sf::SoundSource::Status::Playing)
            {
                footstepSound.stop();
            }
            walkTimer = 0.f;
            setPlayerTexture("baleStand");
        }

        if (movementHintVisible)
        {
            movementHintTimer += dt;
        }

        if (introActive && introDialogIndex == BackpackIntroIndex && !introRewardFading)
        {
            introRewardAlpha = std::min(255.f, introRewardAlpha + 900.f * dt);
        }

        if (introRewardFading)
        {
            introRewardAlpha -= 520.f * dt;
            if (introRewardAlpha <= 0.f)
            {
                introRewardAlpha = 0.f;
                introRewardFading = false;
            }
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
                setPlayerTexture("baleStand");
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
            if (introRewardAlpha > 0.f)
            {
                drawRewardCards(introRewardAlpha);
            }

            if (introActive)
            {
                window.draw(dialogBox);
                if (hasFont)
                {
                    dialogText.setString(utf8(introDialog[introDialogIndex]));
                    window.draw(dialogText);
                    window.draw(dialogHintText);
                }
            }
            else if (movementHintVisible && hasFont)
            {
                const float alphaWave = 0.55f + 0.45f * std::sin(movementHintTimer * 4.5f);
                movementHintText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(150.f + 105.f * alphaWave)));
                const auto bounds = movementHintText.getLocalBounds();
                movementHintText.setPosition({
                    (static_cast<float>(WindowWidth) - bounds.size.x) * 0.5f,
                    805.f
                });
                window.draw(movementHintText);
            }

            if (settingsPopupOpen)
            {
                drawSettingsPopup();
            }
        }
        else
        {
            drawBackpackScene(mousePos);
        }

        if (fadeAlpha > 0.f)
        {
            window.draw(fadeOverlay);
        }

        window.display();
    }

    return 0;
}
