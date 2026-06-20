#pragma once

#include "Constants.h"
#include "Tools/AudioManager.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

class TextHintManager
{
public:
    bool rewardAnimTriggered = false;
    bool needToShowCard = true;
    TextHintManager();
    ~TextHintManager() = default;

    void Initialize(AudioManager *audioMgr);

    void StartDialog();
    void StartDialog(const std::vector<std::string> &dialogs);
    void AdvanceDialog();
    void EndDialog();

    bool IsActive() const { return isActive; }
    std::size_t GetCurrentIndex() const { return currentIndex; }
    const std::string &GetCurrentText() const;

    bool ShouldShowRewardCards()
    {
        if (needToShowCard && isActive && currentIndex == INTRO_BACKPACK_INDEX)
        {
            rewardAnimTriggered = true;
            needToShowCard = false;
            return true;
        }
        return false;
    }
    bool ShouldStartRewardAnimation()
    {
        if (rewardAnimTriggered && isActive && currentIndex > INTRO_BACKPACK_INDEX)
        {
            rewardAnimTriggered = false;
            return true;
        }
        return false;
    }

    bool IsMovementHintVisible() const { return movementHintVisible; }
    void ShowMovementHint();
    void HideMovementHint();
    void UpdateMovementHint(float dt);

    void ShowDoorHint();
    void HideDoorHint();
    void UpdateDoorHint(float dt);

    bool IsDoorHintVisible() const { return doorHintVisible; }
    const std::string &GetDoorHintText() const { return DOOR_HINT_TEXT; }
    float GetDoorHintAlpha() const { return doorHintAlpha; }

    void Update(float dt);

private:
    std::vector<std::string> dialogTexts;
    bool isActive = false;
    std::size_t currentIndex = 0;

    bool movementHintVisible = false;
    float movementHintTimer = 0.f;

    // 门提示
    bool doorHintVisible = false;

    float doorHintTimer = 0.f;
    float doorHintAlpha = 0.f;

    static constexpr float DOOR_HINT_DURATION = 2.5f;
    static constexpr float DOOR_FADE_SPEED = 300.f;

    static const std::string DOOR_HINT_TEXT;

    AudioManager *audioManager = nullptr;

    void ResetState();
    void PlayDialogSound();
    void PlayCompletionSound();
};
