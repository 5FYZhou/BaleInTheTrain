#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "ResourceManager.h"

enum class PanelType { Setting, Backpack, Discard, DealCard, CardsInHand };

class Panel {
protected:
    bool visible = false;
    std::vector<GameEvent>& events;

public:
    Panel(std::vector<GameEvent>& e) : events(e){}
    virtual ~Panel() = default;

    virtual void Update(float dt) {}
    //virtual void Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos) {}

    virtual bool HandleMousePressed(const sf::Vector2f& mousePos){ return false; }

    virtual void Open() { visible = true; }
    virtual void Close() { visible = false; }

    bool IsVisible() const { return visible; }
};

enum class SliderTarget{ None, Music, Sfx };

class SettingPanel : public Panel{
private:
    float musicVolume = DEFAULT_MUSIC_VOLUME;
    float sfxVolume = DEFAULT_SFX_VOLUME;

    SliderTarget draggingSlider = SliderTarget::None;

    const sf::Font* font = nullptr;

    float SliderValueFromX(float x, float left, float width) const;

    void DrawSlider(sf::RenderWindow& window, const std::string& label,
        float value, sf::Vector2f pos);

public:
    std::optional<sf::Sprite> panel, closeButton;

    SettingPanel(std::vector<GameEvent>& e) : Panel(e){}
    ~SettingPanel(){ delete font; }

    void Init(ResourceManager& rm, const sf::Font* uiFont);

    void Draw(sf::RenderWindow& window) ;

    bool HandleMousePressed(const sf::Vector2f& mousePos);
    void HandleMouseMoved(const sf::Vector2f& mousePos);
    void HandleMouseReleased();

    void Close() override {
        visible = false;
        draggingSlider = SliderTarget::None;
    }

    void SetMusicVolume(float v);
    void SetSfxVolume(float v);
};

class BackpackPanel : public Panel {
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;

    const sf::Font* font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    std::optional<sf::Sprite> background, backButton;

    BackpackPanel(std::vector<GameEvent>& e) : Panel(e) {}
    ~BackpackPanel(){ delete rm; delete font; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);

    bool HandleMousePressed(const sf::Vector2f& mousePos);

    void Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos);
};

class DiscardPilePanel : public Panel{
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;

    const sf::Font* font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    std::optional<sf::Sprite> backButton;

    DiscardPilePanel(std::vector<GameEvent>& e) : Panel(e) {}
    ~DiscardPilePanel(){ delete rm; delete font; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);

    bool HandleMousePressed(const sf::Vector2f& mousePos);

    void Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos);
};

class DealCardPanel : public Panel{
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;

    const sf::Font* font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    std::optional<sf::Sprite> backButton;

    DealCardPanel(std::vector<GameEvent>& e) : Panel(e) {}
    ~DealCardPanel(){ delete rm; delete font; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);

    bool HandleMousePressed(const sf::Vector2f& mousePos);

    void Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos);
};

class CardsInHandPanel : public Panel{
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;
    PileType selectedCard;

    const sf::Font* font = nullptr;
    bool hasFont = false;

public:
    CardsInHandPanel(std::vector<GameEvent>& e) : Panel(e) {}
    ~CardsInHandPanel(){ delete rm; delete font; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);
    PileType GetSelectedCard() { return selectedCard; }

    bool HandleMousePressed(const sf::Vector2f& mousePos);
    void HandleMouseMoved(const sf::Vector2f& mousePos);

    void Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos);
};


