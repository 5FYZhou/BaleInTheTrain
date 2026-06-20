#include "UI/Panel.h"

#include <algorithm>
#include <unordered_map>
#include <string>
#include <cmath>

#pragma region 设置
void SettingPanel::Init(ResourceManager& rm, const sf::Font* uiFont){
    font = uiFont;
    
    panel.emplace(rm.getTexture(TextureType::SettingsPanel));
    panel->setPosition({560.f, 235.f});
    //panel->setPosition({628.5f, 321.f});
    
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

float SettingPanel::SliderValueFromX(float x, float left, float width) const{
    return std::clamp((x - left) / width, 0.f, 1.f);
}

void SettingPanel::SetMusicVolume(float v){
    musicVolume = std::clamp(v, 0.f, 1.f);

    musicSlider.value = musicVolume;
    musicValue->setString(std::to_string((int)(musicVolume * 100)) + "%");

    GameEvent e;
    e.type = EventType::MusicVolumeChange;
    e.val = musicVolume;
    events.push_back(e);
}

void SettingPanel::SetSfxVolume(float v){
    sfxVolume = std::clamp(v, 0.f, 1.f);

    sfxSlider.value = sfxVolume;
    sfxValue->setString(std::to_string((int)(sfxVolume * 100)) + "%");

    GameEvent e;
    e.type = EventType::SfxVolumeChange;
    e.val = sfxVolume;
    events.push_back(e);
}

bool SettingPanel::HandleMousePressed(const sf::Vector2f& mousePos){
    if (!visible)
        return false;

    if (closeButton->getGlobalBounds().contains(mousePos)){
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

    if (musicBounds.contains(mousePos)){
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

bool SettingPanel::HandleMouseMoved(const sf::Vector2f& mousePos){
    if (!visible)
        return false;

    if (draggingSlider == SliderTarget::Music){
        SetMusicVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));
    }

    if (draggingSlider == SliderTarget::Sfx){
        SetSfxVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));
    }

    return true;
}

bool SettingPanel::HandleMouseReleased(){
    if(draggingSlider != SliderTarget::None){
        draggingSlider = SliderTarget::None;
        return true;
    }
    return false;
}

void SettingPanel::DrawSlider(sf::RenderWindow& window, const Slider& slider){
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

void SettingPanel::Draw(sf::RenderWindow& window){
    if (!visible) return;

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
static bool PointInRect(const sf::Vector2f& p, const sf::FloatRect& r){
    return r.contains(p);
}

void BackpackPanel::Init(ResourceManager& rmm, const sf::Font* uiFont){
    rm = &rmm;
    font = uiFont;
    hasFont = (font != nullptr);


    background.emplace(rm->getTexture(TextureType::BackpackInterior));
    background->setPosition({0.f, 0.f});
    scaleToWindow(*background);

    backButton.emplace(rm->getTexture(TextureType::BackButton));
    backButton->setPosition({1695.f, 25.f});
    backButton->setScale({0.7f, 0.7f});

    //veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));

    titleText.emplace(*font); 
    titleText->setString(utf8("背包")); 
    titleText->setCharacterSize(60); 
    titleText->setFillColor(sf::Color::Black); 
    titleText->setPosition({780.f, 250.f}); 
    centerTextX(*titleText);
}

void BackpackPanel::SetCards(const std::vector<PileType>& c){
    cards.clear();

    int n = (int)c.size();
    if (n == 0) return;

    float baseX = 960.f;
    float baseY = 525.f;
    float spacingX = 128.f;

    float center = (n - 1) / 2.f;

    for (int i = 0; i < n; i++)
    {
        float offset = i - center;
        float t = (n == 1) ? 0.f : offset / center; // [-1,1]

        RenderCard cv;
        cv.texType = cardTexMap.at(c[i]);
        cv.sprite.emplace(rm->getTexture(cv.texType));

        const auto size = cv.sprite->getTexture().getSize();
        cv.sprite->setOrigin({size.x * 0.5f, size.y * 0.5f});
        cv.sprite->setScale({0.58f, 0.58f});

        // =========================
        // ✔ 位置（对称弧）
        // =========================
        cv.pos = {
            baseX + offset * spacingX,
            baseY + (t * t) * 40.f
        };

        // =========================
        // ✔ 旋转（真正对称关键）
        // =========================
        cv.rotation = t * 12.f;

        cards.push_back(cv);
    }
}

bool BackpackPanel::HandleMousePressed(const sf::Vector2f& mousePos){
    if(!visible) return false;
    if (backButton->getGlobalBounds().contains(mousePos)) {
        //std::cout<<"close backpackpanel"<<std::endl;
        Close();
        return true;
    }
    return true;
}

bool BackpackPanel::HandleMouseMoved(const sf::Vector2f& mousePos){
    for (int i = 0; i < cards.size(); ++i){
        auto& card = cards[i];
        card.hovering = false;
        // hover
        if (card.sprite->getGlobalBounds().contains(mousePos)){
            card.hovering = true;
        }
    }
    return true;
}

void BackpackPanel::Draw(sf::RenderWindow& window){
    if(!visible) return;

    window.draw(veil);
    window.draw(*background);
    window.draw(*backButton);

    for (auto& card : cards)
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

    if (hasFont) {
        window.draw(*titleText);
    }
}
#pragma endregion



#pragma region 发牌池,弃牌堆
void DiscardPilePanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    backButton.emplace(rm->getTexture(TextureType::BackButton));
    backButton->setPosition({1700.f, 900.f});
    backButton->setScale({0.7f, 0.7f});

    //veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));

    titleText.emplace(*font); 
    titleText->setString(utf8("弃牌池")); 
    titleText->setCharacterSize(60); 
    titleText->setFillColor(sf::Color::White); 
    titleText->setPosition({780.f, 120.f}); 
    centerTextX(*titleText);
}

void DiscardPilePanel::SetCards(const std::vector<PileType>& c){
    cards.clear();
    for(size_t i = 0; i < c.size(); ++i){
        int row = static_cast<int>(i) / cardsPerRow;
        int col = static_cast<int>(i) % cardsPerRow;

        CardView cv;
        cv.texType = cardTexMap.at(c[i]);

        cv.basePosition = {
            startX + col * spacingX,
            startY + row * spacingY
        };

        cards.push_back(cv);
    }
}

bool DiscardPilePanel::HandleMousePressed(const sf::Vector2f& mousePos){
    if(!visible) return false;
    if (backButton->getGlobalBounds().contains(mousePos)) {
        //std::cout<<"close backpackpanel"<<std::endl;
        Close();
        return true;
    }
    return true;
}

void DiscardPilePanel::Draw(sf::RenderWindow& window){
    if(!visible) return;

    window.draw(veil);
    window.draw(*backButton);

    for (const auto& card : cards)
    {
        sf::Sprite sprite(rm->getTexture(card.texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
        sprite.setPosition(card.basePosition);
        sprite.setRotation(sf::degrees(card.rotation));
        sprite.setScale({0.58f, 0.58f});

        window.draw(sprite);
    }

    if (hasFont) {
        window.draw(*titleText);
    }
}






void DealCardPanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    backButton.emplace(rm->getTexture(TextureType::BackButton));
    backButton->setPosition({100.f, 900.f});
    backButton->setScale({0.7f, 0.7f});

    //veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));

    titleText.emplace(*font); 
    titleText->setString(utf8("发牌池")); 
    titleText->setCharacterSize(60); 
    titleText->setFillColor(sf::Color::White); 
    titleText->setPosition({750.f, 120.f}); 
    centerTextX(*titleText);
}

void DealCardPanel::SetCards(const std::vector<PileType>& c){
    cards.clear();
    for(size_t i = 0; i < c.size(); ++i){
        int row = static_cast<int>(i) / cardsPerRow;
        int col = static_cast<int>(i) % cardsPerRow;

        CardView cv;
        cv.texType = cardTexMap.at(c[i]);

        cv.basePosition = {
            startX + col * spacingX,
            startY + row * spacingY
        };

        cards.push_back(cv);
    }
}

bool DealCardPanel::HandleMousePressed(const sf::Vector2f& mousePos){
    if(!visible) return false;
    if (backButton->getGlobalBounds().contains(mousePos)) {
        //std::cout<<"close backpackpanel"<<std::endl;
        Close();
        return true;
    }
    return true;
}

void DealCardPanel::Draw(sf::RenderWindow& window){
    if(!visible) return;

    window.draw(veil);
    window.draw(*backButton);

    for (const auto& card : cards)
    {
        sf::Sprite sprite(rm->getTexture(card.texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
        sprite.setPosition(card.basePosition);
        sprite.setRotation(sf::degrees(card.rotation));
        sprite.setScale({0.58f, 0.58f});

        window.draw(sprite);
    }

    if (hasFont) {
        window.draw(*titleText);
    }
}
#pragma endregion



#pragma region 手牌显示
void CardsInHandPanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);
    actionPoints.emplace(rm->getTexture(TextureType::StatusBox));
    actionPoints->setScale({0.5f, 0.8f});
    actionPoints->setPosition({350.f, 750.f});

    
    pointText.emplace(*font);
    pointText->setFillColor(sf::Color::Black);
    //pointText->setStyle(sf::Text::Bold);
    pointText->setCharacterSize(29);
    pointText->setPosition({360.f, 765.f});
}

void CardsInHandPanel::SetCards(const std::vector<PileType>& c, int point, bool first){
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
    if (first){
        cards.clear();

        for (int i = 0; i < c.size(); i++){
            CardView cv;
            cv.cardType = c[i];
            cv.texType = cardTexMap.at(c[i]);

            sf::Vector2f pos;
            float rot;

            ComputeArcTransform(i, c.size(), pos, rot);

            cv.basePosition = pos;
            cv.rotation = rot;          // ⭐关键修复
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
            if (usedOld[i]) continue;

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

    cv.startScale = 0.2f;   // ⭐从小飞入（效果才明显）
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

void CardsInHandPanel::UpdateLayoutArc(){
    int n = 0;
    int total = 0;

    for (auto& c : cards)
        if (c.state == CardAnimState::Idle)
            total++;

    for (auto& c : cards)
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
    for (auto& c : cards)
        if (c.state == CardAnimState::Idle)
            totalIdle++;

    int enteringIndex = 0;

    for (auto& c : cards)
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
            [](const CardView& c){
                return c.state == CardAnimState::Exiting &&
                       c.animTime >= 1.f;
            }),
        cards.end()
    );
}

bool CardsInHandPanel::HandleMousePressed(const sf::Vector2f& mousePos)
{
    if (!visible)
        return false;

    // Do not depend on a prior MouseMoved event. This also makes a direct
    // click reliable when the battle scene has just finished fading in.
    //HandleMouseMoved(mousePos);

    if (hoveredIndex != -1)
    {
        if (!CanInteract(cards[hoveredIndex]))
            return false; // ⭐动画中不能选

        selectedIndex = hoveredIndex;
        selectedCard = cards[selectedIndex].cardType;
        hasSelectedCard = true;
        return true;
    }
    return false;
}

bool CardsInHandPanel::HandleMouseMoved(const sf::Vector2f& mousePos)
{
    if (!visible) return false;

    hoveredIndex = -1;

    for (int i = (int)cards.size() - 1; i >= 0; --i)
    {
        if (!CanInteract(cards[i]))
            continue;

        const auto& c = cards[i];

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

void CardsInHandPanel::Draw(sf::RenderWindow& window){
    if(!visible) return;

    window.draw(*actionPoints);

    if (font){
        pointText->setString(utf8("行动点：") + std::to_string(points));
        window.draw(*pointText);
    }

    for(int i = 0; i < cards.size(); i++) {
        sf::Sprite sprite(rm->getTexture(cards[i].texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x*0.5f, size.y*0.5f});

        sf::Vector2f pos = cards[i].basePosition;
        float rot = cards[i].rotation;
        float scale = cards[i].scale;

        // hover & shift other cards
        if(hoveredIndex != -1) {
            int offset = i - hoveredIndex;
            if(offset != 0) {
                pos.x += offset * 40.f;  // 左右偏移
                pos.y += std::abs(offset) * 4.f; // Y 微调
            }
        }

        // hover 浮起
        bool interactable = CanInteract(cards[i]);

        if (interactable && i == hoveredIndex) {
            pos.y -= 44.f;
            scale = 0.74f;   // 更自然（不要覆盖动画 scale）
        }

        if (interactable && i == selectedIndex) {
            pos.y -= 40.f;
        }

        sprite.setPosition(pos);
        sprite.setRotation(sf::degrees(rot));
        sprite.setScale({scale, scale});
        window.draw(sprite);
    }
}
#pragma endregion

