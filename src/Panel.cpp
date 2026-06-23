#include "UI/Panel.h"

#include <algorithm>
#include <unordered_map>
#include <string>
#include <cmath>

#pragma region 设置
void SettingPanel::Init(ResourceManager &rm, const sf::Font *uiFont)
{
    font = uiFont;

    panel.emplace(rm.getTexture(TextureType::SettingsPanel));
    panel->setPosition({560.f, 235.f});
    // panel->setPosition({628.5f, 321.f});

    closeButton.emplace(rm.getTexture(TextureType::CloseButton));
    closeButton->setPosition({1280.f, 280.f});
    closeButton->setScale({0.65f, 0.65f});

    // ===== title =====
    title.emplace(*font);
    title->setString(utf8("设置"));
    title->setCharacterSize(42);
    title->setPosition({820.f, 315.f});

    // ===== music =====
    musicLabel.emplace(*font);
    musicLabel->setString(utf8("音乐"));
    musicLabel->setCharacterSize(26);

    musicValue.emplace(*font);
    musicValue->setCharacterSize(24);
    musicValue->setString(std::to_string((int)(musicVolume * 100)) + "%");

    // ===== sfx =====
    sfxLabel.emplace(*font);
    sfxLabel->setString(utf8("音效"));
    sfxLabel->setCharacterSize(26);

    sfxValue.emplace(*font);
    sfxValue->setCharacterSize(24);
    sfxValue->setString(std::to_string((int)(sfxVolume * 100)) + "%");

    // ===== sliders =====
    musicSlider.pos = {720.f, 465.f};
    musicSlider.label = ("音乐");
    musicSlider.value = musicVolume;
    musicSlider.valueText = &*musicValue;

    sfxSlider.pos = {720.f, 575.f};
    sfxSlider.label = ("音效");
    sfxSlider.value = sfxVolume;
    sfxSlider.valueText = &*sfxValue;
}

float SettingPanel::SliderValueFromX(float x, float left, float width) const
{
    return std::clamp((x - left) / width, 0.f, 1.f);
}

void SettingPanel::SetMusicVolume(float v)
{
    musicVolume = std::clamp(v, 0.f, 1.f);

    musicSlider.value = musicVolume;
    musicValue->setString(std::to_string((int)(musicVolume * 100)) + "%");

    GameEvent e;
    e.type = EventType::MusicVolumeChange;
    e.val = musicVolume;
    events.push_back(e);
}

void SettingPanel::SetSfxVolume(float v)
{
    sfxVolume = std::clamp(v, 0.f, 1.f);

    sfxSlider.value = sfxVolume;
    sfxValue->setString(std::to_string((int)(sfxVolume * 100)) + "%");

    GameEvent e;
    e.type = EventType::SfxVolumeChange;
    e.val = sfxVolume;
    events.push_back(e);
}

bool SettingPanel::HandleMousePressed(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;

    if (closeButton->getGlobalBounds().contains(mousePos))
    {
        Close();
        return true;
    }

    float left = musicSlider.pos.x;
    float width = 360.f;
    float top = musicSlider.pos.y + 34.f;
    float height = 20.f;

    sf::FloatRect musicBounds({left, top}, {width, height});

    left = sfxSlider.pos.x;
    top = sfxSlider.pos.y + 34.f;

    sf::FloatRect sfxBounds({left, top}, {width, height});

    if (musicBounds.contains(mousePos))
    {
        draggingSlider = SliderTarget::Music;

        SetMusicVolume(SliderValueFromX(mousePos.x, musicSlider.pos.x, 360.f));
        return true;
    }

    if (sfxBounds.contains(mousePos))
    {
        draggingSlider = SliderTarget::Sfx;

        SetSfxVolume(SliderValueFromX(mousePos.x, sfxSlider.pos.x, 360.f));
        return true;
    }

    return true;
}

bool SettingPanel::HandleMouseMoved(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;

    if (draggingSlider == SliderTarget::Music)
    {
        SetMusicVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));
    }

    if (draggingSlider == SliderTarget::Sfx)
    {
        SetSfxVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));
    }

    return true;
}

bool SettingPanel::HandleMouseReleased()
{
    if (draggingSlider != SliderTarget::None)
    {
        draggingSlider = SliderTarget::None;
        return true;
    }
    return false;
}

void SettingPanel::DrawSlider(sf::RenderWindow &window, const Slider &slider)
{
    sf::RectangleShape track({360.f, 8.f});
    track.setPosition({slider.pos.x, slider.pos.y + 34.f});
    track.setFillColor(sf::Color(54, 57, 64, 235));
    window.draw(track);

    sf::RectangleShape fill({360.f * slider.value, 8.f});
    fill.setPosition(track.getPosition());
    fill.setFillColor(sf::Color(224, 186, 92, 245));
    window.draw(fill);

    sf::CircleShape knob(16.f);
    knob.setOrigin({16.f, 16.f});
    knob.setPosition({slider.pos.x + 360.f * slider.value, slider.pos.y + 38.f});
    knob.setFillColor(sf::Color(245, 238, 218));
    window.draw(knob);

    // label（仍然建议缓存成 sf::Text，但这里轻量构造）
    sf::Text labelText(*font);
    labelText.setString(utf8(slider.label));
    labelText.setCharacterSize(26);
    labelText.setPosition({slider.pos.x - 100.f, slider.pos.y + 16.f});
    window.draw(labelText);

    // value（零分配）
    slider.valueText->setPosition({slider.pos.x + 390.f, slider.pos.y + 18.f});
    slider.valueText->setFillColor(sf::Color(235, 235, 235));
    window.draw(*slider.valueText);
}

void SettingPanel::Draw(sf::RenderWindow &window)
{
    if (!visible)
        return;

    sf::RectangleShape veil({WINDOW_WIDTH, WINDOW_HEIGHT});
    veil.setFillColor(sf::Color(0, 0, 0, 115));
    window.draw(veil);

    window.draw(*panel);
    window.draw(*closeButton);

    window.draw(*title);

    DrawSlider(window, musicSlider);
    DrawSlider(window, sfxSlider);
}
#pragma endregion

#pragma region 背包
static bool PointInRect(const sf::Vector2f &p, const sf::FloatRect &r)
{
    return r.contains(p);
}

void BackpackPanel::Init(ResourceManager &rmm, const sf::Font *uiFont)
{
    rm = &rmm;
    font = uiFont;
    hasFont = (font != nullptr);

    background.emplace(rm->getTexture(TextureType::BackpackInterior));
    background->setPosition({0.f, 0.f});
    scaleToWindow(*background);

    backButton.emplace(rm->getTexture(TextureType::BackButton));
    backButton->setPosition({1695.f, 25.f});
    backButton->setScale({0.7f, 0.7f});

    // veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));

    titleText.emplace(*font);
    titleText->setString(utf8("背包"));
    titleText->setCharacterSize(60);
    titleText->setFillColor(sf::Color::Black);
    titleText->setPosition({780.f, 150.f});
    centerTextX(*titleText);
}

void BackpackPanel::SetCards(const std::vector<PileType> &c)
{
    cards.clear();

    int n = (int)c.size();
    if (n == 0)
        return;

    constexpr int MAX_PER_ROW = 10;

    float baseX = 960.f;
    float baseY = 525.f;

    float spacingX = 128.f;
    float spacingY = 200.f;

    int rowCountTotal = (n + MAX_PER_ROW - 1) / MAX_PER_ROW;

    // ✔ 多行整体居中偏移（关键）
    float totalHeight = (rowCountTotal - 1) * spacingY;
    float startY = baseY - totalHeight * 0.5f;

    for (int i = 0; i < n; i++)
    {
        int row = i / MAX_PER_ROW;
        int col = i % MAX_PER_ROW;

        int rowCount = std::min(MAX_PER_ROW, n - row * MAX_PER_ROW);

        float center = (rowCount - 1) * 0.5f;
        float offset = col - center;

        float t = (rowCount <= 1)
                      ? 0.f
                      : offset / center;

        RenderCard cv;
        cv.texType = cardTexMap.at(c[i]);
        cv.sprite.emplace(rm->getTexture(cv.texType));

        const auto size = cv.sprite->getTexture().getSize();

        cv.sprite->setOrigin({size.x * 0.5f,
                              size.y * 0.5f});

        cv.sprite->setScale({0.58f,
                             0.58f});

        cv.pos = {
            baseX + offset * spacingX,
            startY + row * spacingY + (t * t) * 40.f};

        cv.rotation = t * 12.f;

        cards.push_back(cv);
    }
}

bool BackpackPanel::HandleMousePressed(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;
    if (backButton->getGlobalBounds().contains(mousePos))
    {
        // std::cout<<"close backpackpanel"<<std::endl;
        Close();
        return true;
    }
    return true;
}

bool BackpackPanel::HandleMouseMoved(const sf::Vector2f &mousePos)
{
    for (int i = cards.size() - 1; i >= 0; --i)
    {
        auto &card = cards[i];
        card.hovering = false;
    }
    for (int i = cards.size() - 1; i >= 0; --i)
    {
        auto &card = cards[i];
        // hover
        if (card.sprite->getGlobalBounds().contains(mousePos))
        {
            card.hovering = true;
            return true;
        }
    }
    return true;
}

void BackpackPanel::Draw(sf::RenderWindow &window)
{
    if (!visible)
        return;

    window.draw(veil);
    window.draw(*background);
    window.draw(*backButton);

    for (auto &card : cards)
    {
        card.sprite->setPosition(card.pos);
        card.sprite->setRotation(sf::degrees(card.rotation));

        // hover
        if (card.hovering)
        {
            card.sprite->move({0.f, -44.f});
            card.sprite->setScale({0.64f, 0.64f});
        }

        window.draw(*card.sprite);
    }

    if (hasFont)
    {
        window.draw(*titleText);
    }
}
#pragma endregion

#pragma region 发牌池,弃牌堆
void DiscardPilePanel::Init(ResourceManager &resource, const sf::Font *uiFont)
{
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    backButton.emplace(rm->getTexture(TextureType::BackButton));
    backButton->setPosition({1730.f, 930.f});
    backButton->setScale({0.7f, 0.7f});

    // veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));

    titleText.emplace(*font);
    titleText->setString(utf8("弃牌池"));
    titleText->setCharacterSize(60);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({780.f, 120.f});
    centerTextX(*titleText);
}

void DiscardPilePanel::SetCards(const std::vector<PileType> &c)
{
    cards.clear();
    for (size_t i = 0; i < c.size(); ++i)
    {
        int row = static_cast<int>(i) / cardsPerRow;
        int col = static_cast<int>(i) % cardsPerRow;

        CardView cv;
        cv.texType = cardTexMap.at(c[i]);

        cv.basePosition = {
            startX + col * spacingX,
            startY + row * spacingY};

        cards.push_back(cv);
    }
}

bool DiscardPilePanel::HandleMousePressed(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;
    if (backButton->getGlobalBounds().contains(mousePos))
    {
        // std::cout<<"close backpackpanel"<<std::endl;
        Close();
        return true;
    }
    return true;
}

void DiscardPilePanel::Draw(sf::RenderWindow &window)
{
    if (!visible)
        return;

    window.draw(veil);
    window.draw(*backButton);

    for (const auto &card : cards)
    {
        sf::Sprite sprite(rm->getTexture(card.texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
        sprite.setPosition(card.basePosition);
        sprite.setRotation(sf::degrees(card.rotation));
        sprite.setScale({0.58f, 0.58f});

        window.draw(sprite);
    }

    if (hasFont)
    {
        window.draw(*titleText);
    }
}

void DealCardPanel::Init(ResourceManager &resource, const sf::Font *uiFont)
{
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    backButton.emplace(rm->getTexture(TextureType::BackButton));
    backButton->setPosition({100.f, 930.f});
    backButton->setScale({0.7f, 0.7f});

    // veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));

    titleText.emplace(*font);
    titleText->setString(utf8("发牌池"));
    titleText->setCharacterSize(60);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({750.f, 120.f});
    centerTextX(*titleText);
}

void DealCardPanel::SetCards(const std::vector<PileType> &c)
{
    cards.clear();
    for (size_t i = 0; i < c.size(); ++i)
    {
        int row = static_cast<int>(i) / cardsPerRow;
        int col = static_cast<int>(i) % cardsPerRow;

        CardView cv;
        cv.texType = cardTexMap.at(c[i]);

        cv.basePosition = {
            startX + col * spacingX,
            startY + row * spacingY};

        cards.push_back(cv);
    }
}

bool DealCardPanel::HandleMousePressed(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;
    if (backButton->getGlobalBounds().contains(mousePos))
    {
        // std::cout<<"close backpackpanel"<<std::endl;
        Close();
        return true;
    }
    return true;
}

void DealCardPanel::Draw(sf::RenderWindow &window)
{
    if (!visible)
        return;

    window.draw(veil);
    window.draw(*backButton);

    for (const auto &card : cards)
    {
        sf::Sprite sprite(rm->getTexture(card.texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
        sprite.setPosition(card.basePosition);
        sprite.setRotation(sf::degrees(card.rotation));
        sprite.setScale({0.58f, 0.58f});

        window.draw(sprite);
    }

    if (hasFont)
    {
        window.draw(*titleText);
    }
}
#pragma endregion

#pragma region 手牌显示
void CardsInHandPanel::Init(ResourceManager &resource, const sf::Font *uiFont)
{
    rm = &resource;
    font = uiFont;
    float pointX = 500.f;
    float pointY = 750.f;
    hasFont = (font != nullptr);
    pointsBG.emplace(rm->getTexture(TextureType::StatusBox));
    pointsBG->setScale({0.5f, 0.8f});
    pointsBG->setPosition({pointX, pointY});

    pointText.emplace(*font);
    pointText->setFillColor(sf::Color::Black);
    // pointText->setStyle(sf::Text::Bold);
    pointText->setCharacterSize(29);
    pointText->setPosition({pointX + 10, pointY + 15});
}

void CardsInHandPanel::SetCards(const std::vector<PileType> &c, int point, bool first)
{
    points = point;
    hasSelectedCard = false;
    hoveredIndex = -1;
    selectedIndex = -1;

    constexpr float enterStartX = 100.f;
    constexpr float enterStartY = 900.f;

    constexpr float exitEndX = 1700.f;
    constexpr float exitEndY = 900.f;

    std::vector<CardView> newCards;
    std::vector<bool> usedOld(cards.size(), false);

    // ⭐ 第一次直接弧形排列
    if (first)
    {
        cards.clear();

        for (int i = 0; i < c.size(); i++)
        {
            CardView cv;
            cv.cardType = c[i];
            cv.texType = cardTexMap.at(c[i]);

            sf::Vector2f pos;
            float rot;

            ComputeArcTransform(i, c.size(), pos, rot);

            cv.basePosition = pos;
            cv.rotation = rot; // ⭐关键修复
            cv.state = CardAnimState::Idle;

            cards.push_back(cv);
        }
        return;
    }

    // ⭐ match existing
    for (auto type : c)
    {
        bool found = false;

        for (int i = 0; i < cards.size(); i++)
        {
            if (usedOld[i])
                continue;

            if (cards[i].cardType == type &&
                cards[i].state != CardAnimState::Exiting)
            {
                CardView cv = cards[i];
                cv.state = CardAnimState::Idle;

                newCards.push_back(cv);
                usedOld[i] = true;
                found = true;
                break;
            }
        }

        // ⭐ 新牌：飞入弧形目标
        if (!found)
        {
            CardView cv;
            cv.cardType = type;
            cv.texType = cardTexMap.at(type);

            cv.state = CardAnimState::Entering;

            cv.animStart = {enterStartX, enterStartY};
            cv.animTime = 0.f;

            // ⭐⭐⭐关键补充（必须）
            cv.startRotation = 0.f;
            cv.rotation = 0.f;

            cv.startScale = 0.2f; // ⭐从小飞入（效果才明显）
            cv.scale = 0.2f;

            newCards.push_back(cv);
        }
    }

    // ⭐ 多余牌：飞出
    for (int i = 0; i < cards.size(); i++)
    {
        if (!usedOld[i] && cards[i].state != CardAnimState::Exiting)
        {
            CardView cv = cards[i];
            cv.state = CardAnimState::Exiting;

            cv.animStart = cv.basePosition; // ⭐弧形起点
            cv.animEnd = {exitEndX, exitEndY};

            cv.animTime = 0.f;

            newCards.push_back(cv);
        }
    }

    cards = std::move(newCards);

    UpdateLayoutArc(); // ⭐关键
}

void CardsInHandPanel::UpdateLayoutArc()
{
    int n = 0;
    int total = 0;

    for (auto &c : cards)
        if (c.state == CardAnimState::Idle)
            total++;

    for (auto &c : cards)
    {
        if (c.state != CardAnimState::Idle)
            continue;

        ComputeArcTransform(n, total, c.basePosition, c.rotation);
        n++;
    }
}

void CardsInHandPanel::Update(float dt)
{
    const float speed = 6.f;

    int totalIdle = 0;
    for (auto &c : cards)
        if (c.state == CardAnimState::Idle)
            totalIdle++;

    int enteringIndex = 0;

    for (auto &c : cards)
    {
        if (c.state == CardAnimState::Idle)
            continue;

        c.animTime += dt * speed;
        float t = std::min(c.animTime, 1.f);
        float s = Smooth(t);

        // ⭐进入动画
        if (c.state == CardAnimState::Entering)
        {
            sf::Vector2f targetPos;
            float targetRot;

            ComputeArcTransform(enteringIndex, totalIdle, targetPos, targetRot);
            enteringIndex++;

            c.basePosition = Lerp(c.animStart, targetPos, s);

            // ⭐ rotation 修复（关键）
            c.rotation = Lerp(0.f, targetRot, s);

            // ⭐ scale（解决问题2核心）
            c.scale = Lerp(0.2f, 0.7f, s);

            if (t >= 1.f)
            {
                c.state = CardAnimState::Idle;
                c.basePosition = targetPos;
                c.rotation = targetRot;
                c.scale = 0.7f;
                UpdateLayoutArc();
            }
        }

        // ⭐出牌动画
        else if (c.state == CardAnimState::Exiting)
        {
            c.basePosition = Lerp(c.animStart, c.animEnd, s);

            // ⭐关键：明显缩小（杀戮尖塔风格）
            c.scale = Lerp(0.7f, 0.1f, s);

            c.rotation = Lerp(c.rotation, c.rotation + 30.f, s);
        }
    }

    // 删除完成出牌
    cards.erase(
        std::remove_if(cards.begin(), cards.end(),
                       [](const CardView &c)
                       {
                           return c.state == CardAnimState::Exiting &&
                                  c.animTime >= 1.f;
                       }),
        cards.end());
}

bool CardsInHandPanel::HandleMousePressed(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;

    // Do not depend on a prior MouseMoved event. This also makes a direct
    // click reliable when the battle scene has just finished fading in.
    // HandleMouseMoved(mousePos);

    if (hoveredIndex != -1)
    {
        if (!CanInteract(cards[hoveredIndex]))
            return false; // ⭐动画中不能选

        selectedIndex = hoveredIndex;
        selectedCard = cards[selectedIndex].cardType;

        hasSelectedCard = true;
        draggingCard = true; // 开始拖拽
        return true;
    }
    return false;
}

bool CardsInHandPanel::HandleMouseMoved(const sf::Vector2f &mousePos)
{
    if (!visible)
        return false;

    hoveredIndex = -1;
    currentMousePos = mousePos;

    for (int i = (int)cards.size() - 1; i >= 0; --i)
    {
        if (!CanInteract(cards[i]))
            continue;

        const auto &c = cards[i];

        sf::Sprite sprite(rm->getTexture(c.texType));
        const auto texSize = sprite.getTexture().getSize();

        float w = texSize.x * c.scale;
        float h = texSize.y * c.scale;

        // 1. 计算中心
        sf::Vector2f center = c.basePosition;

        // 2. 平移到中心坐标
        sf::Vector2f d = mousePos - center;

        float rad = c.rotation * 3.1415926f / 180.f;

        // 3. 逆旋转
        float cosA = std::cos(-rad);
        float sinA = std::sin(-rad);

        sf::Vector2f local;
        local.x = d.x * cosA - d.y * sinA;
        local.y = d.x * sinA + d.y * cosA;

        // 4. 判断 AABB（注意 scale）
        if (std::abs(local.x) <= w * 0.5f &&
            std::abs(local.y) <= h * 0.5f)
        {
            hoveredIndex = i;
            return true;
        }
    }

    return false;
}

void DrawSegment(
    sf::RenderWindow &window,
    sf::Vector2f a,
    sf::Vector2f b,
    float thickness,
    sf::Color color)
{
    sf::Vector2f dir = b - a;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.001f)
        return;

    dir /= len;

    sf::RectangleShape rect;
    rect.setSize({len, thickness});
    rect.setOrigin({0.f, thickness * 0.5f});
    rect.setPosition(a);

    rect.setRotation(sf::radians(std::atan2(dir.y, dir.x)));

    rect.setFillColor(color);

    window.draw(rect);
}

void CardsInHandPanel::DrawArrow(
    sf::RenderWindow &window,
    const sf::Vector2f &start,
    const sf::Vector2f &end)
{
    sf::Vector2f delta = end - start;

    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    if (dist < 10.f)
        return;

    // =========================
    // 默认颜色（黄色）
    // =========================
    sf::Color arrowColor = sf::Color(255, 220, 50);

    // =========================
    // 当前卡牌
    // =========================
    if (selectedIndex < 0 || selectedIndex >= (int)cards.size())
        return;

    PileType card = cards[selectedIndex].cardType;

    // =========================
    // 判断卡牌目标类型
    // =========================
    bool canPlayer = CanPlayer(card);
    bool canEnemy = CanEnemy(card);

    // =========================
    // 当前鼠标命中目标
    // =========================
    bool onPlayer = playerCollider.contains(end);
    bool onEnemy = enemyCollider.contains(end);

    // =========================
    // 规则：合法 => 绿色，否则红色
    // =========================
    if ((canPlayer && onPlayer) ||
        (canEnemy && onEnemy))
    {
        arrowColor = sf::Color(80, 255, 120); // 绿色（合法）
    }
    else if((canPlayer && onEnemy) ||
        (canEnemy && onPlayer))
    {
        arrowColor = sf::Color(255, 80, 80); // 红色（非法）
    }

    // =========================
    // 贝塞尔控制点
    // =========================
    sf::Vector2f p0 = start;
    sf::Vector2f p2 = end;

    sf::Vector2f p1(
        (start.x + end.x) * 0.5f,
        std::min(start.y, end.y) - 180.f);

    // =========================
    // 采样曲线
    // =========================
    constexpr int SEGMENTS = 30;

    std::vector<sf::Vector2f> pts;
    pts.reserve(SEGMENTS + 1);

    for (int i = 0; i <= SEGMENTS; i++)
    {
        float t = (float)i / SEGMENTS;
        float u = 1.f - t;

        sf::Vector2f p(
            u * u * p0.x + 2 * u * t * p1.x + t * t * p2.x,
            u * u * p0.y + 2 * u * t * p1.y + t * t * p2.y);

        pts.push_back(p);
    }

    // =========================
    // 粗线绘制
    // =========================
    constexpr float thickness = 8.f;

    for (size_t i = 1; i < pts.size(); i++)
    {
        float t = (float)i / pts.size();

        sf::Color c(
            arrowColor.r,
            arrowColor.g,
            arrowColor.b,
            static_cast<std::uint8_t>(80 + 175 * t));

        DrawSegment(
            window,
            pts[i - 1],
            pts[i],
            thickness,
            c);
    }

    // =========================
    // 箭头方向
    // =========================
    sf::Vector2f dir = pts.back() - pts[pts.size() - 2];

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.001f)
        return;

    dir /= len;

    sf::Vector2f normal(-dir.y, dir.x);

    // =========================
    // 箭头头
    // =========================
    constexpr float arrowLength = 30.f;
    constexpr float arrowWidth = 16.f;

    sf::ConvexShape arrow;
    arrow.setPointCount(3);

    arrow.setPoint(0, p2);
    arrow.setPoint(1,
                   p2 - dir * arrowLength + normal * arrowWidth);
    arrow.setPoint(2,
                   p2 - dir * arrowLength - normal * arrowWidth);

    arrow.setFillColor(arrowColor);

    window.draw(arrow);
}

void CardsInHandPanel::Draw(sf::RenderWindow &window)
{
    if (!visible)
        return;

    window.draw(*pointsBG);

    if (font)
    {
        pointText->setString(utf8("行动点：") + std::to_string(points));
        window.draw(*pointText);
    }

    for (int i = 0; i < cards.size(); i++)
    {
        sf::Sprite sprite(rm->getTexture(cards[i].texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});

        sf::Vector2f pos = cards[i].basePosition;
        float rot = cards[i].rotation;
        float scale = cards[i].scale;

        // hover & shift other cards
        if (hoveredIndex != -1)
        {
            int offset = i - hoveredIndex;
            if (offset != 0)
            {
                pos.x += offset * 40.f;          // 左右偏移
                pos.y += std::abs(offset) * 4.f; // Y 微调
            }
        }

        // hover 浮起
        bool interactable = CanInteract(cards[i]);

        if (interactable && i == hoveredIndex)
        {
            pos.y -= 44.f;
            scale = 0.74f; // 更自然（不要覆盖动画 scale）
        }

        if (interactable && i == selectedIndex)
        {
            pos.y -= 40.f;
        }

        sprite.setPosition(pos);
        sprite.setRotation(sf::degrees(rot));
        sprite.setScale({scale, scale});
        window.draw(sprite);
    }

    if (ShouldShowArrow())
    {
        sf::Vector2f start =
            cards[selectedIndex].basePosition;

        start.y -= 120.f;

        DrawArrow(window, start, currentMousePos);
    }
}
#pragma endregion

#pragma region buff显示
void BuffPanel::SetBuff(
    PlanType intent, int intentNum, int enemyDefendNum,
    const std::vector<std::pair<BuffDebuffType, int>> &enemyBuff,
    const std::vector<std::pair<BuffDebuffType, int>> &playerBuff,
    int playerDefendNum)
{
    enemyIntent = intent;
    enemyIntentNum = intentNum;
    enemyDefend = enemyDefendNum;

    playerDefend = playerDefendNum;

    enemyBuffs.clear();
    playerBuffs.clear();

    auto convert = [&](const std::vector<std::pair<BuffDebuffType, int>> &src,
                       std::vector<BuffIcon> &dst)
    {
        for (auto &b : src)
        {
            BuffIcon icon;
            icon.type = b.first;
            icon.value = b.second;
            icon.tex = buffTexMap.at(b.first);
            dst.push_back(icon);
        }
    };

    convert(enemyBuff, enemyBuffs);
    convert(playerBuff, playerBuffs);
}

bool BuffPanel::HandleMouseMoved(const sf::Vector2f &pos)
{
    hoveredBuff = nullptr;
    hoveredDefaultBuff = false;
    hoveredIntent = false;

    // defaultBuff
    if (defaultBuff)
    {
        if (defaultBuff->getGlobalBounds().contains(pos))
        {
            hoveredDefaultBuff = true;
            return false;
        }
    }

    // 敌人意图
    if (intentBounds.contains(pos))
    {
        hoveredIntent = true;
        return false;
    }

    for (auto &buff : enemyBuffs)
    {
        if (buff.bounds.contains(pos))
        {
            hoveredBuff = &buff;
            return false;
        }
    }

    for (auto &buff : playerBuffs)
    {
        if (buff.bounds.contains(pos))
        {
            hoveredBuff = &buff;
            return false;
        }
    }

    return false;
}

void BuffPanel::DrawIcon(sf::RenderWindow &window, TextureType tex, sf::Vector2f pos, sf::Vector2f scope)
{
    sf::Sprite sp(rm->getTexture(tex));
    CenterOrigin(sp);
    FitSprite(sp, scope);
    sp.setPosition(pos);
    if (tex == TextureType::p_power_up_player)
        sp.setRotation(sf::degrees(45));

    window.draw(sp);
}

void BuffPanel::DrawIconWithNum(sf::RenderWindow &window, TextureType tex, int num, sf::Vector2f pos,
                                sf::Vector2f size, int offsetX, int offsetY, int fontsize)
{
    DrawIcon(window, tex, pos, size);

    if (num < 0)
        return;
    sf::Text text(*font);
    text.setString(std::to_string(num));

    text.setCharacterSize(fontsize);
    text.setFillColor(sf::Color::White);

    auto b = text.getLocalBounds();
    text.setOrigin({b.position.x + b.size.x / 2.f,
                    b.position.y + b.size.y / 2.f});

    text.setPosition({pos.x + offsetX, pos.y + offsetY});

    window.draw(text);
}

void BuffPanel::DrawTooltip(sf::RenderWindow &window)
{
    std::string info;
    sf::Vector2f anchor;
    bool showBelow = false;

    if (hoveredDefaultBuff)
    {
        info = defaultBuffInfo;
        anchor = defaultBuff->getPosition();
        showBelow = true;
    }
    else if (hoveredIntent)
    {
        auto it = planInfoMap.find(enemyIntent);

        if (it == planInfoMap.end())
            return;

        info = it->second;

        anchor = {
            intentBounds.position.x +
                intentBounds.size.x * 0.5f,

            intentBounds.position.y +
                intentBounds.size.y * 0.5f};
    }
    else if (hoveredBuff)
    {
        auto it = buffInfoMap.find(hoveredBuff->type);

        if (it == buffInfoMap.end())
            return;

        info = it->second;
        anchor = hoveredBuff->pos;
    }
    else
    {
        return;
    }

    sf::Text text(*font);
    text.setString(utf8(info));
    text.setCharacterSize(22);
    text.setFillColor(sf::Color::Black);

    auto tb = text.getLocalBounds();

    constexpr float padding = 10.f;

    float width =
        tb.size.x + padding * 2.f;

    float height =
        tb.size.y + padding * 2.f;

    sf::Vector2f pos{anchor.x,
                     showBelow ? anchor.y + 120.f : anchor.y - 60.f};

    constexpr float screenW = 1920.f;
    constexpr float screenH = 1080.f;

    // 左边界
    if (pos.x - width * 0.5f < 0.f)
        pos.x = width * 0.5f;

    // 右边界
    if (pos.x + width * 0.5f > screenW)
        pos.x = screenW - width * 0.5f;

    // 上边界
    if (pos.y - height * 0.5f < 0.f)
        pos.y = height * 0.5f;

    // 下边界
    if (pos.y + height * 0.5f > screenH)
        pos.y = screenH - height * 0.5f;

    sf::RectangleShape bg;

    bg.setSize({width,
                height});

    bg.setFillColor(
        sf::Color(255, 255, 255, 240));

    bg.setOrigin({width * 0.5f,
                  height * 0.5f});

    bg.setPosition(pos);

    text.setOrigin({tb.position.x + tb.size.x * 0.5f,
                    tb.position.y + tb.size.y * 0.5f});

    text.setPosition(pos);

    window.draw(bg);
    window.draw(text);
}

void BuffPanel::Draw(sf::RenderWindow &window)
{
    if (!visible)
        return;

    if (defaultBuff)
    {
        defaultBuff->setColor(
            hoveredDefaultBuff
                ? sf::Color(255, 255, 255, 180)
                : sf::Color::White);

        window.draw(*defaultBuff);
    }

    // 1. 敌人意图
    sf::Vector2f intentPos = enemyPos;
    intentPos.x += 70;
    intentPos.y -= 30;

    DrawIconWithNum(window, planTexMap.at(enemyIntent),
                    enemyIntentNum, intentPos, {50, 50}, 40);

    sf::Vector2f enemyBuffPos = enemyBound.position;
    enemyBuffPos.x += 10;
    enemyBuffPos.y = enemyPos.y + enemyBound.size.y + enemyHPDrawOffset + 40;

    // 敌人防御
    if (enemyDefend > 0)
    {
        DrawIconWithNum(window, TextureType::p_defend_player, enemyDefend,
                        {enemyBuffPos.x - 20, enemyBuffPos.y - 70}, {50, 50}, 40);
    }
    int col = 0;

    // 2. 敌人 buff
    for (auto &b : enemyBuffs)
    {
        sf::Vector2f drawPos{
            enemyBuffPos.x + col * space,
            enemyBuffPos.y};

        b.pos = drawPos;

        b.bounds = {
            {drawPos.x - 20, drawPos.y - 20},
            {40, 40}};

        DrawIconWithNum(window, b.tex, b.value, drawPos, {40, 40}, 15, 10, 28);
        col++;
    }

    // 3. 玩家 buff
    sf::Vector2f playerBuffPos = playerPos;
    playerBuffPos.x -= 110;
    playerBuffPos.y += 50;

    col = 0;

    if (playerDefend > 0)
    {
        DrawIconWithNum(
            window,
            TextureType::p_defend_player,
            playerDefend,
            {playerPos.x - 110, playerPos.y - 30},
            {50, 50},
            40);
    }

    for (auto &b : playerBuffs)
    {
        sf::Vector2f drawPos{
            playerBuffPos.x + col * space,
            playerBuffPos.y};

        b.pos = drawPos;

        b.bounds = {
            {drawPos.x - 20, drawPos.y - 20},
            {40, 40}};

        DrawIconWithNum(window, b.tex, b.value, drawPos, {40, 40}, 15, 10, 28);

        col++;
    }
    DrawTooltip(window);
}
#pragma endregion
