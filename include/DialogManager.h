#pragma once

#include "Constants.h"
#include <string>
#include <vector>

class AudioManager;

class DialogManager {
public:
    DialogManager();
    ~DialogManager() = default;

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

    void Update(float dt);

private:
    std::vector<std::string> dialogTexts;
    bool isActive = false;
    std::size_t currentIndex = 0;

    float introRewardAlpha = 0.f;
    bool introRewardFading = false;

    bool movementHintVisible = false;
    float movementHintTimer = 0.f;

    AudioManager* audioManager = nullptr;

    void ResetState();
    void PlayDialogSound();
    void PlayCompletionSound();
};
