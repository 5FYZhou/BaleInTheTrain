#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "Constants.h"
#include "Tools/ResourceManager.h"

enum class PanelType
{
    None = 0,
    Setting,
    Backpack,
    Discard,
    DealCard,
    CardsInHand,
    Buff
};
enum class PanelLayer
{
    Bottom = 0,
    Middle,
    Top,
    Count
};

class Panel
{
protected:
    PanelType type;
    PanelLayer layer;
    bool visible = false;
    std::vector<GameEvent> &events;

public:
    Panel(std::vector<GameEvent> &e) : events(e) {}
    virtual void Update(float dt) {}
    virtual void Draw(sf::RenderWindow &) = 0;

    virtual bool HandleMousePressed(const sf::Vector2f &) { return false; }
    virtual bool HandleMouseMoved(const sf::Vector2f &) { return false; }
    virtual bool HandleMouseReleased() { return false; }

    virtual bool BlocksInput() const { return visible; }
    virtual void Open() { visible = true; }
    virtual void Close() { visible = false; }

    bool IsVisible() const { return visible; }
    PanelType GetID() { return type; }
    PanelLayer GetLayer() const { return layer; }
};

#pragma region 设置
enum class SliderTarget
{
    None,
    Music,
    Sfx
};

class SettingPanel : public Panel
{
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
    struct Slider
    {
        sf::Vector2f pos;
        std::string label;
        float value;
        sf::Text *valueText;
    };

    Slider musicSlider;
    Slider sfxSlider;

    const sf::Font *font = nullptr;

    float SliderValueFromX(float x, float left, float width) const;

    void DrawSlider(sf::RenderWindow &window, const Slider &slider);

public:
    SettingPanel(std::vector<GameEvent> &e) : Panel(e)
    {
        type = PanelType::Setting;
        layer = PanelLayer::Top;
    }

    void Init(ResourceManager &rm, const sf::Font *uiFont);

    void Draw(sf::RenderWindow &window) override;

    bool HandleMousePressed(const sf::Vector2f &mousePos) override;
    bool HandleMouseMoved(const sf::Vector2f &mousePos) override;
    bool HandleMouseReleased() override;

    void Close() override
    {
        visible = false;
        draggingSlider = SliderTarget::None;
    }

    void SetMusicVolume(float v);
    void SetSfxVolume(float v);
};
#pragma endregion

#pragma region 背包
class BackpackPanel : public Panel
{
private:
    ResourceManager *rm = nullptr;

    struct RenderCard
    {
        PileType type;
        TextureType texType;
        std::optional<sf::Sprite> sprite;
        sf::Vector2f pos;
        float rotation = 0.f;
        bool hovering = false;
    };

    std::vector<RenderCard> cards;

    std::optional<sf::Sprite> background, backButton;
    std::optional<sf::Text> titleText;
    const sf::Font *font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    BackpackPanel(std::vector<GameEvent> &e) : Panel(e)
    {
        type = PanelType::Backpack;
        layer = PanelLayer::Middle;
    }
    void Init(ResourceManager &resource, const sf::Font *uiFont);

    void SetCards(const std::vector<PileType> &c);

    bool HandleMousePressed(const sf::Vector2f &mousePos) override;
    bool HandleMouseMoved(const sf::Vector2f &) override;

    void Draw(sf::RenderWindow &window) override;
};
#pragma endregion

#pragma region 发牌池,弃牌堆
inline constexpr int cardsPerRow = 8;
inline constexpr float startX = 400.f;
inline constexpr float startY = 350.f;
inline constexpr float spacingX = 150.f;
inline constexpr float spacingY = 230.f;

class DiscardPilePanel : public Panel
{
private:
    ResourceManager *rm = nullptr;

    std::vector<CardView> cards;

    std::optional<sf::Sprite> backButton;
    std::optional<sf::Text> titleText;
    const sf::Font *font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    DiscardPilePanel(std::vector<GameEvent> &e) : Panel(e)
    {
        type = PanelType::Discard;
        layer = PanelLayer::Middle;
    }
    void Init(ResourceManager &resource, const sf::Font *uiFont);

    void SetCards(const std::vector<PileType> &c);

    bool HandleMousePressed(const sf::Vector2f &mousePos) override;
    bool HandleMouseMoved(const sf::Vector2f &mousePos) override { return visible; }

    void Draw(sf::RenderWindow &window) override;
};

class DealCardPanel : public Panel
{
private:
    ResourceManager *rm = nullptr;

    std::vector<CardView> cards;

    std::optional<sf::Sprite> backButton;
    std::optional<sf::Text> titleText;
    const sf::Font *font = nullptr;
    bool hasFont = false;

    sf::RectangleShape veil; // 遮罩用 RectangleShape 更合理

public:
    DealCardPanel(std::vector<GameEvent> &e) : Panel(e)
    {
        type = PanelType::DealCard;
        layer = PanelLayer::Middle;
    }
    void Init(ResourceManager &resource, const sf::Font *uiFont);

    void SetCards(const std::vector<PileType> &c);

    bool HandleMousePressed(const sf::Vector2f &mousePos) override;
    bool HandleMouseMoved(const sf::Vector2f &mousePos) override { return visible; }

    void Draw(sf::RenderWindow &window) override;
};
#pragma endregion

#pragma region 手牌显示
class CardsInHandPanel : public Panel
{
private:
    ResourceManager *rm = nullptr;

    std::vector<CardView> cards;
    bool hasSelectedCard = false;
    PileType selectedCard = PileType::Strike;
    int hoveredIndex = -1;
    int selectedIndex = -1;

    int points = 0;

    std::optional<sf::Sprite> pointsBG;
    std::optional<sf::Text> pointText;
    const sf::Font *font = nullptr;
    bool hasFont = false;

    float Smooth(float t) { return t * t * (3.f - 2.f * t); }
    sf::Vector2f Lerp(sf::Vector2f a, sf::Vector2f b, float t) { return a + (b - a) * t; }
    float Lerp(float a, float b, float t) { return a + (b - a) * t; }

    void UpdateLayoutArc();
    bool CanInteract(const CardView &c) { return c.state == CardAnimState::Idle; }
    void ComputeArcTransform(int index, int n, sf::Vector2f &pos, float &rot)
    {
        float centerX = 960.f;
        float baseY = 980.f;

        float spacingX = 120.f;
        float liftY = 6.f;

        float maxAngle = 15.f;

        float offset = index - (n - 1) / 2.f;

        pos = {
            centerX + offset * spacingX,
            baseY + std::abs(offset) * liftY};

        rot = offset * (maxAngle / (n > 1 ? (n - 1) / 2.f : 1.f));
    }

public:
    CardsInHandPanel(std::vector<GameEvent> &e) : Panel(e)
    {
        type = PanelType::CardsInHand;
        layer = PanelLayer::Bottom;
    }
    void Init(ResourceManager &resource, const sf::Font *uiFont);
    void Update(float dt) override;

    void SetCards(const std::vector<PileType> &c, int point, bool first = false);
    std::pair<PileType, int> GetSelectedCard() { return {selectedCard, selectedIndex}; }
    bool HasSelectedCard() { return visible && hasSelectedCard; }
    void SetHasSelected(bool f)
    {
        if (!visible)
        {
            std::cout << "CardsInHandPanel:unvidible set" << std::endl;
            return;
        }
        hasSelectedCard = f;
        selectedIndex = -1;
        hoveredIndex = -1;
    }

    bool HandleMousePressed(const sf::Vector2f &mousePos) override;
    bool HandleMouseMoved(const sf::Vector2f &mousePos) override;

    void Draw(sf::RenderWindow &window) override;

    bool BlocksInput() const override { return false; }
};
#pragma endregion

#pragma region buff显示
class BuffPanel : public Panel
{
public:
    struct BuffIcon
{
    TextureType tex;
    BuffDebuffType type;

    int value = 0;
    bool hasValue = false;

    sf::Vector2f pos;      // 实际绘制位置
    sf::FloatRect bounds;  // 鼠标检测区域
};

private:
    // ===== 数据 =====
    PlanType enemyIntent = PlanType::None;
    int enemyIntentNum = 0;
    int enemyDefend = 0;

    std::vector<BuffIcon> enemyBuffs;
    std::vector<BuffIcon> playerBuffs;

    int playerDefend = 0;

    // ===== 外部上下文 =====
    sf::Vector2f playerPos;
    sf::Vector2f enemyPos;
    sf::FloatRect enemyBound;
    float enemyHPDrawOffset = 0.f;

    // ===== 资源 =====
    ResourceManager* rm = nullptr;
    const sf::Font* font = nullptr;

    // ===== layout =====
    float space = 50.f;

    // 悬浮显示信息
    const BuffIcon* hoveredBuff = nullptr;

private:
    void DrawIcon(sf::RenderWindow& window, TextureType tex, sf::Vector2f pos, sf::Vector2f size);

    void DrawIconWithNum(sf::RenderWindow& window, TextureType tex, int num, sf::Vector2f pos,
                         sf::Vector2f size, int offset, int offsetY = 0, int fontsize = 30);
    void DrawTooltip(sf::RenderWindow& window);

public:
    BuffPanel(std::vector<GameEvent>& e)
        : Panel(e)
    {
        type = PanelType::Buff;
        layer = PanelLayer::Bottom;
    }

    void Init(ResourceManager& res, const sf::Font* f)
    {
        rm = &res;
        font = f;
    }

    void SetContext(
        const sf::Vector2f& playerPos,
        const sf::Vector2f& enemyPos,
        const sf::FloatRect& enemyBound,
        float enemyHPOffset)
    {
        this->playerPos = playerPos;
        this->enemyPos = enemyPos;
        this->enemyBound = enemyBound;
        this->enemyHPDrawOffset = enemyHPOffset;
    }

    void SetBuff(
        PlanType enemyIntent,
        int enemyIntentNum, int enemyDefendNum,
        const std::vector<std::pair<BuffDebuffType,int>>& enemyBuff,
        const std::vector<std::pair<BuffDebuffType,int>>& playerBuff,
        int playerDefendNum = 0);

    bool HandleMouseMoved(const sf::Vector2f& pos) override ;

    void Draw(sf::RenderWindow& window) override;
};
#pragma endregion


class PanelManager
{
private:
    // 生命周期管理
    std::vector<std::unique_ptr<Panel>> allPanels;

    // 类型查找
    std::unordered_map<PanelType, Panel *> panelMap;

    // 每层按打开顺序存储
    std::array<std::vector<Panel *>, static_cast<size_t>(PanelLayer::Count)> layerPanels;

public:
    template <typename T, typename... Args>
    T *AddPanel(Args &&...args)
    {
        auto ptr =
            std::make_unique<T>(std::forward<Args>(args)...);

        T *raw = ptr.get();

        allPanels.push_back(std::move(ptr));

        panelMap[raw->GetID()] = raw;

        return raw;
    }

    template <typename T>
    T *Get()
    {
        for (auto &p : allPanels)
        {
            if (auto ptr = dynamic_cast<T *>(p.get()))
            {
                return ptr;
            }
        }

        return nullptr;
    }

    void Open(PanelType id)
    {
        auto it = panelMap.find(id);
        if (it == panelMap.end())
            return;

        Panel *panel = it->second;

        if (panel->IsVisible())
            return;

        panel->Open();

        auto &layer = layerPanels[(size_t)panel->GetLayer()];

        // ⭐ 防止重复加入
        if (std::find(layer.begin(), layer.end(), panel) == layer.end())
        {
            layer.push_back(panel);
        }
    }

    void Close(PanelType id)
    {
        auto it = panelMap.find(id);

        if (it == panelMap.end())
            return;

        Panel *panel = it->second;

        if (!panel->IsVisible()) return;

        panel->Close();

        auto &layer = layerPanels[static_cast<size_t>(panel->GetLayer())];

        layer.erase(
            std::remove(
                layer.begin(),
                layer.end(),
                panel),
            layer.end());
    }

    void CloseAll()
    {
        for (auto &panel : allPanels)
        {
            if (panel->IsVisible())
            {
                panel->Close();
            }
        }

        for (auto &layer : layerPanels)
        {
            layer.clear();
        }
    }

    bool IsOpen(PanelType id) const
    {
        auto it = panelMap.find(id);

        return it != panelMap.end() && it->second->IsVisible();
    }

    bool HandleMousePressed(const sf::Vector2f &pos)
    {
        for (int l = static_cast<int>(PanelLayer::Count) - 1; l >= 0; --l)
        {
            auto &layer = layerPanels[l];

            for (auto it = layer.rbegin(); it != layer.rend(); ++it)
            {
                Panel *panel = *it;

                if (panel->IsVisible() && panel->HandleMousePressed(pos))
                    return true;
            }
        }

        return false;
    }

    bool HandleMouseMoved(const sf::Vector2f &pos)
    {
        for (int l = static_cast<int>(PanelLayer::Count) - 1; l >= 0; --l)
        {
            auto &layer = layerPanels[l];

            for (auto it = layer.rbegin(); it != layer.rend(); ++it)
            {
                Panel *panel = *it;

                if (panel->IsVisible() && panel->HandleMouseMoved(pos))
                    return true;
            }
        }

        return false;
    }

    bool HandleMouseReleased()
    {
        for (int l = static_cast<int>(PanelLayer::Count) - 1; l >= 0; --l)
        {
            auto &layer = layerPanels[l];

            for (auto it = layer.rbegin(); it != layer.rend(); ++it)
            {
                Panel *panel = *it;

                if (panel->IsVisible() && panel->HandleMouseReleased())
                    return true;
            }
        }

        return false;
    }

    void Update(float dt)
    {
        for (auto &layer : layerPanels)
        {
            for (auto *panel : layer)
            {
                panel->Update(dt);
            }
        }
    }

    void Draw(sf::RenderWindow &window)
    {
        for (size_t l = 0; l < static_cast<size_t>(PanelLayer::Count); ++l)
        {
            for (auto *panel : layerPanels[l])
            {
                panel->Draw(window);
            }
        }
    }

    bool BlocksInput() const
    {
        for (int l = static_cast<int>(PanelLayer::Count) - 1; l >= 0; --l)
        {
            auto const &layer = layerPanels[l];

            for (auto it = layer.rbegin(); it != layer.rend(); ++it)
            {
                if ((*it)->BlocksInput())
                    return true;
            }
        }

        return false;
    }
};
