#include "Panel.h"

#include <algorithm>
#include <unordered_map>
#include <string>

void SettingPanel::Init(ResourceManager& rm, const sf::Font* uiFont)
{
    font = uiFont;
    panel->setPosition({560.f, 235.f});

    closeButton->setPosition({1280.f, 280.f});
    closeButton->setScale({0.65f, 0.65f});
}

float SettingPanel::SliderValueFromX(float x, float left, float width) const{
    return std::clamp((x - left) / width, 0.f, 1.f);
}

void SettingPanel::SetMusicVolume(float v){
    musicVolume = std::clamp(v, 0.f, 1.f);
    GameEvent e;
    e.type = EventType::MusicVolumeChange;
    e.val = musicVolume;
    events.push_back(e);
}

void SettingPanel::SetSfxVolume(float v){
    sfxVolume = std::clamp(v, 0.f, 1.f);
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

    sf::FloatRect musicBounds({735.f, 482.f}, {410.f, 58.f});

    sf::FloatRect sfxBounds({735.f, 592.f}, {410.f, 58.f});

    if (musicBounds.contains(mousePos)){
        draggingSlider = SliderTarget::Music;

        SetMusicVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));

        return true;
    }

    if (sfxBounds.contains(mousePos))
    {
        draggingSlider = SliderTarget::Sfx;

        SetSfxVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));

        return true;
    }

    return true;
}

void SettingPanel::HandleMouseMoved(const sf::Vector2f& mousePos)
{
    if (!visible)
        return;

    if (draggingSlider == SliderTarget::Music){
        SetMusicVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));
    }

    if (draggingSlider == SliderTarget::Sfx){
        SetSfxVolume(SliderValueFromX(mousePos.x, 720.f, 360.f));
    }
}

void SettingPanel::HandleMouseReleased(){
    draggingSlider = SliderTarget::None;
}

void SettingPanel::DrawSlider(sf::RenderWindow& window, const std::string& label, float value, sf::Vector2f pos){
    sf::RectangleShape track({360.f, 8.f});

    track.setPosition({pos.x, pos.y + 34.f});

    track.setFillColor(sf::Color(54, 57, 64, 235));

    window.draw(track);

    sf::RectangleShape fill({360.f * value, 8.f});

    fill.setPosition(track.getPosition());

    fill.setFillColor(sf::Color(224, 186, 92, 245));

    window.draw(fill);

    sf::CircleShape knob(16.f);

    knob.setOrigin({16.f,16.f});

    knob.setPosition({ pos.x + 360.f * value, pos.y + 38.f });
    knob.setFillColor(sf::Color(245, 238, 218));

    window.draw(knob);

    if (font){
        sf::Text txt(*font);

        txt.setString(label);
        txt.setCharacterSize(26);
        txt.setPosition({ pos.x - 100.f, pos.y + 16.f });

        window.draw(txt);

        sf::Text valueTxt(*font);
        valueTxt.setFillColor(sf::Color(235, 235, 235));

        valueTxt.setString(std::to_string(static_cast<int>(value * 100.f)) + "%");

        valueTxt.setCharacterSize(24);

        valueTxt.setPosition({ pos.x + 390.f, pos.y + 18.f });

        window.draw(valueTxt);
    }
}

void SettingPanel::Draw(sf::RenderWindow& window) {
    if (!visible)
        return;

    sf::RectangleShape veil({WINDOW_WIDTH, WINDOW_HEIGHT});

    veil.setFillColor(sf::Color(0, 0, 0, 115));

    window.draw(veil);

    window.draw(*panel);
    window.draw(*closeButton);

    if (font){
        sf::Text title(*font);

        title.setString("Settings");

        title.setCharacterSize(42);

        title.setPosition({820.f, 315.f});

        window.draw(title);
    }

    DrawSlider(window, "Music", musicVolume, {720.f, 465.f});

    DrawSlider(window, "SFX", sfxVolume, {720.f, 575.f});
}








static bool PointInRect(const sf::Vector2f& p, const sf::FloatRect& r){
    return r.contains(p);
}

void BackpackPanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    //background .emplace(sf::Sprite(resource.getTexture(TextureType::BackpackInterior)));
    background->setPosition({0.f, 0.f});

    //backButton.emplace(sf::Sprite(resource.getTexture(TextureType::BackButton)));
    backButton->setPosition({1695.f, 25.f});
    backButton->setScale({0.7f, 0.7f});

    //veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));
}

void BackpackPanel::SetCards(const std::vector<PileType>& c){
    cards.clear();
    int k1 = 0, k2 = 0;
    for(int i = 0; i < c.size(); i++){
        CardView cv;
        cv.texType = cardTexMap.at(c[i]);
        if(c[i] == PileType::Strike){
            cv.basePosition = {410.f + k1 * 128.f, 525.f + k1 * 7.f}; // 位置（X轴间隔128，Y轴微调） 
            cv.rotation = -7.f + k1 * 1.5f; // 旋转（扇形展开）
            cards.push_back(cv);
            k1++;
        }
    }
    for(int i = 0; i < c.size(); i++){
        CardView cv;
        cv.texType = cardTexMap.at(c[i]);
        if(c[i] == PileType::Defend){
            cv.basePosition = {1010.f + k2 * 128.f, 525.f + k2 * 7.f}; // 位置（X轴间隔128，Y轴微调） 
            cv.rotation = -7.f + k2 * 1.5f; // 旋转（扇形展开）
            cards.push_back(cv);
            k2++;
        }
    }
    if(k1 + k2 < c.size()){
        std::cout<<"BackpackPanel:has undefined draw type"<<std::endl;
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

void BackpackPanel::Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos){
    if(!visible) return;

    sf::Vector2f mouseF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    window.draw(veil);
    window.draw(*background);
    window.draw(*backButton);

    for (const auto& card : cards)
    {
        sf::Sprite sprite(rm->getTexture(card.texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
        sprite.setPosition(card.basePosition);
        sprite.setRotation(sf::degrees(card.rotation));
        sprite.setScale({0.58f, 0.58f});

        // hover
        if (sprite.getGlobalBounds().contains(mouseF))
        {
            sprite.move({0.f, -44.f});
            sprite.setScale({0.64f, 0.64f});
        }

        window.draw(sprite);
    }

    if (hasFont) {
        sf::Text titleText(*font); 
        titleText.setString("Backpack"); 
        titleText.setCharacterSize(60); 
        titleText.setFillColor(sf::Color::Black); 
        titleText.setPosition({780.f, 250.f}); 
        window.draw(titleText);
    }
}





void DiscardPilePanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    //backButton.emplace(sf::Sprite(resource.getTexture(TextureType::BackButton)));
    backButton->setPosition({1700.f, 900.f});
    backButton->setScale({0.7f, 0.7f});

    //veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));
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

void DiscardPilePanel::Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos){
    if(!visible) return;

    sf::Vector2f mouseF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

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

        // hover
        //if (sprite.getGlobalBounds().contains(mouseF))
        //{
        //    sprite.move({0.f, -44.f});
        //    sprite.setScale({0.64f, 0.64f});
        //}

        window.draw(sprite);
    }

    if (hasFont) {
        sf::Text titleText(*font); 
        titleText.setString("Discard Pile"); 
        titleText.setCharacterSize(60); 
        titleText.setFillColor(sf::Color::White); 
        titleText.setPosition({780.f, 100.f}); 
        window.draw(titleText);
    }
}






void DealCardPanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);

    //backButton.emplace(sf::Sprite(resource.getTexture(TextureType::BackButton)));
    backButton->setPosition({100.f, 900.f});
    backButton->setScale({0.7f, 0.7f});

    //veil = sf::Sprite();    // 遮罩初始化
    veil.setSize({1920.f, 1080.f});
    veil.setFillColor(sf::Color(0, 0, 0, 100));
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

void DealCardPanel::Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos){
    if(!visible) return;

    sf::Vector2f mouseF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

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

        // hover
        //if (sprite.getGlobalBounds().contains(mouseF))
        //{
        //    sprite.move({0.f, -44.f});
        //    sprite.setScale({0.64f, 0.64f});
        //}

        window.draw(sprite);
    }

    if (hasFont) {
        sf::Text titleText(*font); 
        titleText.setString("Deal Card Pile"); 
        titleText.setCharacterSize(60); 
        titleText.setFillColor(sf::Color::White); 
        titleText.setPosition({750.f, 100.f}); 
        window.draw(titleText);
    }
}




void CardsInHandPanel::Init(ResourceManager& resource, const sf::Font* uiFont){
    rm = &resource;
    font = uiFont;
    hasFont = (font != nullptr);
}

void CardsInHandPanel::SetCards(const std::vector<PileType>& c, int point, bool first){
    points = point;
    hoveredIndex = -1;
    selectedIndex = -1;

    constexpr float enterStartX = 100.f;
    constexpr float enterStartY = 900.f;

    constexpr float exitEndX = 1700.f;
    constexpr float exitEndY = 900.f;

    std::vector<CardView> newCards;
    std::vector<bool> usedOld(cards.size(), false);

    // =========================
    // ⭐ 第一次直接弧形排列
    // =========================
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

    // =========================
    // ⭐ match existing
    // =========================
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

    // =========================
    // ⭐ 多余牌：飞出
    // =========================
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

        // =========================
        // ⭐进入动画
        // =========================
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

        // =========================
        // ⭐出牌动画
        // =========================
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

void CardsInHandPanel::HandleMouseMoved(const sf::Vector2f& mousePos)
{
    if (!visible) return;

    hoveredIndex = -1;

    for (int i = 0; i < cards.size(); i++)
    {
        if (!CanInteract(cards[i]))
            continue; // ⭐动画中直接跳过

        sf::Sprite sprite(rm->getTexture(cards[i].texType));
        const auto size = sprite.getTexture().getSize();

        sf::FloatRect bounds(
            {cards[i].basePosition.x - size.x * 0.5f,
             cards[i].basePosition.y - size.y * 0.5f},
            {static_cast<float>(size.x),
             static_cast<float>(size.y)}
        );

        if (bounds.contains(mousePos))
        {
            hoveredIndex = i;
            break;
        }
    }
}

void CardsInHandPanel::Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos){
    if(!visible) return;
    sf::Vector2f mouseF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    actionPoints->setScale({0.5f, 0.8f});
    actionPoints->setPosition({350.f, 750.f});
    window.draw(*actionPoints);

    if (font){
        sf::Text title(*font);

        title.setFillColor(sf::Color::Black);
        title.setString("points:" + std::to_string(points));

        title.setCharacterSize(30);

        title.setPosition({360.f, 765.f});

        window.draw(title);
    }

    for(int i = 0; i < cards.size(); i++)
    {
        sf::Sprite sprite(rm->getTexture(cards[i].texType));
        const auto size = sprite.getTexture().getSize();
        sprite.setOrigin({size.x*0.5f, size.y*0.5f});

        sf::Vector2f pos = cards[i].basePosition;
        float rot = cards[i].rotation;
        float scale = cards[i].scale;

        // hover & shift other cards
        if(hoveredIndex != -1)
        {
            int offset = i - hoveredIndex;
            if(offset != 0)
            {
                pos.x += offset * 40.f;  // 左右偏移
                pos.y += std::abs(offset) * 4.f; // Y 微调
            }
        }

        // hover 浮起
bool interactable = CanInteract(cards[i]);

if (interactable && i == hoveredIndex)
{
    pos.y -= 44.f;
    scale = 0.74f;   // 更自然（不要覆盖动画 scale）
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
}


