#include "Panel.h"

#include <algorithm>

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
    backButton->setPosition({50.f, 55.f});
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
        titleText.setString("backpack"); 
        titleText.setCharacterSize(50); 
        titleText.setFillColor(sf::Color::White); 
        titleText.setPosition({145.f, 62.f}); 
        window.draw(titleText);
    }
}

