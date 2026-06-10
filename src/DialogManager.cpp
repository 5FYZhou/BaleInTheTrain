#include "DialogManager.h"
#include "AudioManager.h"

#include <algorithm>

namespace {
std::vector<std::string> DefaultIntroDialog() {
    return {
        "Bale:\n...Where am I? After that impact, the whole carriage went strangely quiet.",
        "Bale:\nWhy is nobody here? The train has stopped. I should try opening the door.",
        "Bale:\nThe door is locked. To leave, I need to find a key or enough fragments to make one.",
        "Bale:\nThere is a backpack here. It has five Strike 6 cards and five Defend 5 cards inside.",
        "Bale:\nThese cards may help in a fight. I will keep them and check the next carriage."
    };
}
}

DialogManager::DialogManager()
    : dialogTexts(DefaultIntroDialog())
{
}

void DialogManager::Initialize(AudioManager* audioMgr)
{
    audioManager = audioMgr;
    ResetState();
}

void DialogManager::ResetState()
{
    isActive = false;
    currentIndex = 0;
    introRewardAlpha = 0.f;
    introRewardFading = false;
    movementHintVisible = false;
    movementHintTimer = 0.f;
}

void DialogManager::StartDialog()
{
    if (dialogTexts.empty()) {
        dialogTexts = DefaultIntroDialog();
    }

    isActive = true;
    currentIndex = 0;
    introRewardAlpha = 0.f;
    introRewardFading = false;
    movementHintVisible = false;
    movementHintTimer = 0.f;
}

void DialogManager::StartDialog(const std::vector<std::string>& dialogs)
{
    dialogTexts = dialogs.empty() ? DefaultIntroDialog() : dialogs;
    StartDialog();
}

void DialogManager::AdvanceDialog()
{
    if (!isActive) {
        return;
    }

    PlayDialogSound();

    if (currentIndex == INTRO_BACKPACK_INDEX) {
        introRewardFading = true;
        introRewardAlpha = 255.f;
    }

    if (currentIndex + 1 < dialogTexts.size()) {
        ++currentIndex;
    } else {
        EndDialog();
    }
}

void DialogManager::EndDialog()
{
    isActive = false;
    movementHintVisible = true;
    movementHintTimer = 0.f;
    PlayCompletionSound();
}

const std::string& DialogManager::GetCurrentText() const
{
    if (currentIndex < dialogTexts.size()) {
        return dialogTexts[currentIndex];
    }

    static const std::string empty;
    return empty;
}

void DialogManager::StartRewardFade()
{
    introRewardFading = true;
    introRewardAlpha = 255.f;
}

void DialogManager::UpdateRewardFade(float dt)
{
    if (introRewardFading) {
        introRewardAlpha -= DIALOG_FADE_OUT_SPEED * dt;
        if (introRewardAlpha <= 0.f) {
            introRewardAlpha = 0.f;
            introRewardFading = false;
        }
    } else if (isActive && currentIndex == INTRO_BACKPACK_INDEX && introRewardAlpha < 255.f) {
        introRewardAlpha = std::min(255.f, introRewardAlpha + DIALOG_FADE_IN_SPEED * dt);
    }
}

void DialogManager::ShowMovementHint()
{
    movementHintVisible = true;
    movementHintTimer = 0.f;
}

void DialogManager::HideMovementHint()
{
    movementHintVisible = false;
}

void DialogManager::UpdateMovementHint(float dt)
{
    if (movementHintVisible) {
        movementHintTimer += dt;
        if (movementHintTimer >= MOVEMENT_HINT_DURATION) {
            movementHintVisible = false;
        }
    }
}

void DialogManager::Update(float dt)
{
    UpdateRewardFade(dt);
    UpdateMovementHint(dt);
}

void DialogManager::PlayDialogSound()
{
    if (audioManager) {
        audioManager->PlaySound(SoundEffect::Dialog);
    }
}

void DialogManager::PlayCompletionSound()
{
    if (audioManager) {
        audioManager->PlaySound(SoundEffect::Tutorial);
    }
}
