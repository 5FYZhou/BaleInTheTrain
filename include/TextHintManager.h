#pragma once

#include "Constants.h"
#include "AudioManager.h"
#include <string>
#include <vector>

class TextHintManager {
public:
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
    bool introRewardFading = false;

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
