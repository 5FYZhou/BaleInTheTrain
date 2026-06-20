#pragma once

#include "Constants.h"
#include "AudioManager.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

class RewardAnimation {
public:
    struct RewardCardAnim {
        CardView card;

        sf::Vector2f startPos;
        sf::Vector2f endPos;

        float startScale = 0.58f;
        float endScale = 0.f;

        float startAlpha = 255.f;
        float endAlpha = 0.f;

        float t = 0.f;
        float duration = 0.7f;

        bool active = false;
        bool done = false;
    };

    std::vector<RewardCardAnim> anims;

private:
    float Ease(float t)
    {
        return t * t * (3.f - 2.f * t);
    }

    sf::Vector2f backpack = {1695.f, 25.f};

    // =========================
    // ⭐ 扇形生成（只负责摆出来）
    // =========================
    CardView MakeFanCard(PileType type, int index, int count,
        sf::Vector2f center, float yOffset,
        float spacingX, float angleRange)
    {
        CardView cv;
        cv.texType = cardTexMap.at(type);

        float mid = (count - 1) / 2.f;
        float offset = index - mid;

        float t = (count <= 1) ? 0.f : offset / mid;

        cv.basePosition = {
            center.x + offset * spacingX,
            center.y + (t * t) * 25.f + yOffset
        };

        cv.rotation = t * angleRange;

        return cv;
    }

public:

    // =========================================================
    // ⭐ ① 入口1：只“出现扇形卡牌”
    // =========================================================
    void Set(const std::vector<PileType>& cards)
    {
        anims.clear();

        sf::Vector2f strikeCenter = {960.f, 380.f};
        sf::Vector2f defendCenter = {960.f, 500.f};

        std::vector<PileType> strike, defend;

        for (auto c : cards)
        {
            if (c == PileType::Strike) strike.push_back(c);
            else defend.push_back(c);
        }

        auto build = [&](std::vector<PileType>& group, sf::Vector2f center)
        {
            int n = (int)group.size();

            for (int i = 0; i < n; i++)
            {
                CardView fan = MakeFanCard(
                    group[i], i, n,
                    center, 0.f, 80.f, 18.f
                );

                RewardCardAnim a;
                a.card.texType = fan.texType;

                // ⭐只负责“显示位置”
                a.startPos = fan.basePosition;
                a.endPos = fan.basePosition; // 不动

                a.card.basePosition = fan.basePosition;

                a.startScale = 0.58f;
                a.endScale = 0.58f;

                a.startAlpha = 255.f;
                a.endAlpha = 255.f;

                a.duration = 0.001f; // 不动画

                a.active = true;

                anims.push_back(a);
            }
        };

        build(strike, strikeCenter);
        build(defend, defendCenter);
    }

    // =========================================================
    // ⭐ ② 入口2：开始“收集动画”
    // =========================================================
    void Start()
    {
        for (auto& a : anims)
        {
            a.startPos = a.card.basePosition; // 当前扇形位置
            a.endPos = backpack;

            a.t = 0.f;
            a.duration = 0.7f;

            a.startScale = 0.58f;
            a.endScale = 0.0f;

            a.startAlpha = 255.f;
            a.endAlpha = 0.f;

            a.active = true;
            a.done = false;
        }
    }

    // =========================================================
    // ⭐ Update（统一执行动画）
    // =========================================================
    void Update(float dt)
    {
        for (auto& a : anims)
        {
            if (a.done) continue;

            if (!a.active) continue;

            a.t += dt / a.duration;
            float t = std::min(a.t, 1.f);
            float s = Ease(t);

            // 位置
            a.card.basePosition =
                a.startPos + (a.endPos - a.startPos) * s;

            // 缩放
            a.card.scale =
                a.startScale + (a.endScale - a.startScale) * s;

            // 透明
            a.card.alpha =
                a.startAlpha + (a.endAlpha - a.startAlpha) * s;

            if (t >= 1.f)
                a.done = true;
        }

        anims.erase(
            std::remove_if(anims.begin(), anims.end(),
                [](auto& a){ return a.done; }),
            anims.end());
    }
};


class TextHintManager {
public:
    RewardAnimation rewardAnim;
    bool introRewardFading = false;
bool rewardAnimTriggered = false;
    TextHintManager();
    ~TextHintManager() = default;

    void Initialize(AudioManager* audioMgr);

    void StartDialog();
    void StartDialog(const std::vector<std::string>& dialogs);
    void AdvanceDialog();
    void EndDialog();

    bool IsActive() const { return isActive; }
    std::size_t GetCurrentIndex() const { return currentIndex; }
    const std::string& GetCurrentText() const;

    bool IsShowingReward() const { return introRewardAlpha > 0.f; }
    float GetRewardAlpha() const { return introRewardAlpha; }
    void StartRewardFade();
    void UpdateRewardFade(float dt);
    bool ShouldShowRewardCards() const
{
    return isActive && currentIndex >= INTRO_BACKPACK_INDEX;
}
bool ShouldStartRewardAnimation() const
{
    // 进入该句对话瞬间触发一次
    return isActive &&
           currentIndex == INTRO_BACKPACK_INDEX &&
           !introRewardFading;
}

    bool IsMovementHintVisible() const { return movementHintVisible; }
    void ShowMovementHint();
    void HideMovementHint();
    void UpdateMovementHint(float dt);

    bool ShouldShowReward() const {
        return isActive && currentIndex == INTRO_BACKPACK_INDEX && !introRewardFading;
    }

    void ShowDoorHint();
    void HideDoorHint();
    void UpdateDoorHint(float dt);

    bool IsDoorHintVisible() const { return doorHintVisible; }
    const std::string& GetDoorHintText() const { return DOOR_HINT_TEXT; }
    float GetDoorHintAlpha() const { return doorHintAlpha; }

    void Update(float dt);

private:
    std::vector<std::string> dialogTexts;
    bool isActive = false;
    std::size_t currentIndex = 0;

    float introRewardAlpha = 0.f;

    bool movementHintVisible = false;
    float movementHintTimer = 0.f;

    // 门提示
    bool doorHintVisible = false;

    float doorHintTimer = 0.f;
    float doorHintAlpha = 0.f;

    static constexpr float DOOR_HINT_DURATION = 2.5f;
    static constexpr float DOOR_FADE_SPEED = 300.f;

    static const std::string DOOR_HINT_TEXT;

    AudioManager* audioManager = nullptr;

    void ResetState();
    void PlayDialogSound();
    void PlayCompletionSound();
};

