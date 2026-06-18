#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "ResourceManager.h"

enum class PanelType { None = 0, Setting, Backpack, Discard, DealCard, CardsInHand };
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

    float scale = 0.7f;
    float startScale = 0.7f;
    float targetScale = 0.7f;

    float startRotation = 0.f;
};

class Panel {
protected:
    PanelType type;
    bool visible = false;
    std::vector<GameEvent>& events;
public:
    Panel(std::vector<GameEvent>& e) : events(e) {}
    virtual void Update(float dt) {}
    virtual void Draw(sf::RenderWindow&) = 0;

    virtual bool HandleMousePressed(const sf::Vector2f&) { return false; }
    virtual void HandleMouseMoved(const sf::Vector2f&) {}
    virtual void HandleMouseReleased() {}

    virtual bool BlocksInput() const { return visible; }
    virtual void Open() { visible = true; }
    virtual void Close() { visible = false; }

    bool IsVisible() const { return visible; }
    PanelType GetID() { return type; }

};

enum class SliderTarget{ None, Music, Sfx };

class SettingPanel : public Panel{
private:
    float musicVolume = DEFAULT_MUSIC_VOLUME;
    float sfxVolume = DEFAULT_SFX_VOLUME;

    SliderTarget draggingSlider = SliderTarget::None;

    std::optional<sf::Sprite> panel, closeButton;
    std::optional<sf::Text> title;
    std::optional<sf::Text> musicLabel;
    std::optional<sf::Text> musicValue;
    std::optional<sf::Text> sfxLabel;
    std::optional<sf::Text> sfxValue;
    // slider layout
    struct Slider {
        sf::Vector2f pos;
        std::string label;
        float value;
        sf::Text* valueText;
    };

    Slider musicSlider;
    Slider sfxSlider;

    const sf::Font* font = nullptr;

    float SliderValueFromX(float x, float left, float width) const;

    void DrawSlider(sf::RenderWindow& window, const Slider& slider);

public:
    SettingPanel(std::vector<GameEvent>& e) : Panel(e){ type = PanelType::Setting; }

    void Init(ResourceManager& rm, const sf::Font* uiFont);

    void Draw(sf::RenderWindow& window) override;

    bool HandleMousePressed(const sf::Vector2f& mousePos) override;
    void HandleMouseMoved(const sf::Vector2f& mousePos) override;
    void HandleMouseReleased() override;

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

    struct RenderCard {
        PileType type;
        TextureType texType;
        std::optional<sf::Sprite> sprite;
        sf::Vector2f pos;
        float rotation = 0.f;
        bool hovering = false;
    };

    std::vector<RenderCard> cards;

    std::optional<sf::Sprite> background, backButton;
    const sf::Font* font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    BackpackPanel(std::vector<GameEvent>& e) : Panel(e) { type = PanelType::Backpack; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);

    bool HandleMousePressed(const sf::Vector2f& mousePos) override;
    void HandleMouseMoved(const sf::Vector2f&) override;

    void Draw(sf::RenderWindow& window) override;
};

#pragma region 发牌池,弃牌堆
inline constexpr int cardsPerRow = 8;
inline constexpr float startX = 400.f;
inline constexpr float startY = 350.f;
inline constexpr float spacingX = 150.f;
inline constexpr float spacingY = 230.f;

class DiscardPilePanel : public Panel{
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;

    std::optional<sf::Sprite> backButton;
    const sf::Font* font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    DiscardPilePanel(std::vector<GameEvent>& e) : Panel(e) { type = PanelType::Discard; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);

    bool HandleMousePressed(const sf::Vector2f& mousePos) override;

    void Draw(sf::RenderWindow& window) override;
};

class DealCardPanel : public Panel{
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;

    std::optional<sf::Sprite> backButton;
    const sf::Font* font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    DealCardPanel(std::vector<GameEvent>& e) : Panel(e) { type = PanelType::DealCard; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);

    void SetCards(const std::vector<PileType>& c);

    bool HandleMousePressed(const sf::Vector2f& mousePos) override;

    void Draw(sf::RenderWindow& window) override;
};
#pragma endregion


#pragma region 手牌显示
class CardsInHandPanel : public Panel{
private:
    ResourceManager* rm = nullptr;

    std::vector<CardView> cards;
    bool hasSelectedCard;
    PileType selectedCard;
    int hoveredIndex = -1;
    int selectedIndex = -1;

    int points;

    std::optional<sf::Sprite> actionPoints;
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
    CardsInHandPanel(std::vector<GameEvent>& e) : Panel(e) { type = PanelType::CardsInHand; }
    void Init(ResourceManager& resource, const sf::Font* uiFont);
    void Update(float dt) override;

    void SetCards(const std::vector<PileType>& c, int point, bool first = false);
    std::pair<PileType, int> GetSelectedCard() { return {selectedCard, selectedIndex}; }
    bool HasSelectedCard() { return visible && hasSelectedCard; }
    void SetHasSelected(bool f) { 
        if(!visible){
            std::cout<<"CardsInHandPanel:unvidible set"<<std::endl;
            return;
        }
        hasSelectedCard = f; 
    }

    bool HandleMousePressed(const sf::Vector2f& mousePos) override;
    void HandleMouseMoved(const sf::Vector2f& mousePos) override;

    void Draw(sf::RenderWindow& window) override;

    bool BlocksInput() const override { return false; }
};
#pragma endregion




class PanelManager {
private:
    std::vector<std::unique_ptr<Panel>> panels;
    std::unordered_map<PanelType, Panel*> panelMap;

public:
    template<typename T, typename... Args>
    T* AddPanel(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();

        panels.push_back(std::move(ptr));
        panelMap[raw->GetID()] = raw;

        return raw;
    }

    template<typename T>
    T* Get() {
        for (auto& p : panels) {
            if (auto ptr = dynamic_cast<T*>(p.get())) {
                return ptr;
            }
        }
        return nullptr;
    }

    void Open(PanelType id) {
        if (panelMap.count(id))
            panelMap[id]->Open();
    }
    void Close(PanelType id) {
        if (panelMap.count(id))
            panelMap[id]->Close();
    }
    bool IsOpen(PanelType id) const {
        auto it = panelMap.find(id);
        return it != panelMap.end() && it->second->IsVisible();
    }

    bool HandleMousePressed(const sf::Vector2f& pos) {
        for (auto it = panels.rbegin(); it != panels.rend(); ++it) {
            if ((*it)->IsVisible() &&
                (*it)->HandleMousePressed(pos))
                return true;
        }
        return false;
    }

    void HandleMouseMoved(const sf::Vector2f& pos) {
        for (auto& p : panels)
            if (p->IsVisible()) p->HandleMouseMoved(pos);
    }

    void HandleMouseReleased() {
        for (auto& p : panels)
            if (p->IsVisible()) p->HandleMouseReleased();
    }

    void Update(float dt) {
        for (auto& p : panels)
            if (p->IsVisible()) p->Update(dt);
    }

    void Draw(sf::RenderWindow& window) {
        for (auto& p : panels)
            if (p->IsVisible()) p->Draw(window);
    }

    bool BlocksInput() const {
        for (auto& p : panels)
            if (p->BlocksInput()) return true;
        return false;
    }
};