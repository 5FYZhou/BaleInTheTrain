#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "ResourceManager.h"

enum class PanelType { Setting, Backpack, Discard, DealCard, CardsInHand };

inline constexpr int cardsPerRow = 8;
inline constexpr float startX = 400.f;
inline constexpr float startY = 350.f;
inline constexpr float spacingX = 150.f;
inline constexpr float spacingY = 230.f;
enum class CardAnimState {
    Idle,
    Entering,
    Exiting
};

struct CardView {
    PileType cardType;
    TextureType texType;

    sf::Vector2f basePosition;
    float rotation = 0.f;

    CardAnimState state = CardAnimState::Idle;

    sf::Vector2f animStart;
    sf::Vector2f animEnd;
    float animTime = 0.f;

    // ⭐新增（解决问题2）
    float scale = 0.7f;
    float startScale = 0.7f;
    float targetScale = 0.7f;

    // ⭐解决 rotation 动画
    float startRotation = 0.f;
};

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
    bool hasSelectedCard;
    PileType selectedCard;
    int hoveredIndex = -1;
    int selectedIndex = -1;

    int points;

    const sf::Font* font = nullptr;
    bool hasFont = false;
    
    float Smooth(float t){ return t * t * (3.f - 2.f * t); }
    sf::Vector2f Lerp(sf::Vector2f a, sf::Vector2f b, float t){ return a + (b - a) * t; }
    float Lerp(float a, float b, float t){ return a + (b - a) * t;}

    void UpdateLayoutArc();
    bool CanInteract(const CardView& c){ return c.state == CardAnimState::Idle; }
    void ComputeArcTransform(int index, int n, sf::Vector2f& pos, float& rot){
        float centerX = 960.f;
        float baseY = 950.f;

        float spacingX = 120.f;
        float liftY = 6.f;

        float maxAngle = 15.f;

        float offset = index - (n - 1) / 2.f;

        pos = {
            centerX + offset * spacingX,
            baseY + std::abs(offset) * liftY
        };

        rot = offset * (maxAngle / (n > 1 ? (n - 1) / 2.f : 1.f));
    }

public:
    std::optional<sf::Sprite> actionPoints;
    CardsInHandPanel(std::vector<GameEvent>& e) : Panel(e) {}
    ~CardsInHandPanel(){ delete rm; delete font; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);
    void Update(float dt) override;

    void SetCards(const std::vector<PileType>& c, int point, bool first = false);
    std::pair<PileType, int> GetSelectedCard() { return {selectedCard, selectedIndex}; }
    bool HasSelectedCard() { return hasSelectedCard; }
    void SetHasSelected(bool f) { hasSelectedCard = f; }

    bool HandleMousePressed(const sf::Vector2f& mousePos);
    void HandleMouseMoved(const sf::Vector2f& mousePos);

    void Draw(sf::RenderWindow& window, const sf::Vector2i& mousePos);
};


