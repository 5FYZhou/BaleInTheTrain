#include "TextHintManager.h"
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
const std::string TextHintManager::DOOR_HINT_TEXT = "A door with three locks";

TextHintManager::TextHintManager()
    : dialogTexts(DefaultIntroDialog())
{
}

void TextHintManager::Initialize(AudioManager* audioMgr)
{
    audioManager = audioMgr;
    ResetState();
}

void TextHintManager::ResetState()
{
    isActive = false;
    currentIndex = 0;
    introRewardAlpha = 0.f;
    introRewardFading = false;
    movementHintVisible = false;
    movementHintTimer = 0.f;
}

void TextHintManager::StartDialog()
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

void TextHintManager::StartDialog(const std::vector<std::string>& dialogs)
{
    dialogTexts = dialogs.empty() ? DefaultIntroDialog() : dialogs;
    StartDialog();
}

void TextHintManager::AdvanceDialog()
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

void TextHintManager::EndDialog()
{
    isActive = false;
    movementHintVisible = true;
    movementHintTimer = 0.f;
    PlayCompletionSound();
}

const std::string& TextHintManager::GetCurrentText() const
{
    if (currentIndex < dialogTexts.size()) {
        return dialogTexts[currentIndex];
    }

    static const std::string empty;
    return empty;
}

void TextHintManager::StartRewardFade()
{
    introRewardFading = true;
    introRewardAlpha = 255.f;
}

void TextHintManager::UpdateRewardFade(float dt)
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

void TextHintManager::ShowMovementHint()
{
    movementHintVisible = true;
    movementHintTimer = 0.f;
}

void TextHintManager::HideMovementHint()
{
    movementHintVisible = false;
}

void TextHintManager::UpdateMovementHint(float dt)
{
    if (movementHintVisible) {
        movementHintTimer += dt;
        if (movementHintTimer >= MOVEMENT_HINT_DURATION) {
            movementHintVisible = false;
        }
    }
}

void TextHintManager::Update(float dt)
{
    UpdateRewardFade(dt);
    UpdateMovementHint(dt);
    UpdateDoorHint(dt);
}

void TextHintManager::PlayDialogSound()
{
    if (audioManager) {
        audioManager->PlaySound(SoundEffect::Dialog);
    }
}

void TextHintManager::PlayCompletionSound()
{
    if (audioManager) {
        audioManager->PlaySound(SoundEffect::Tutorial);
    }
}


void TextHintManager::ShowDoorHint()
{
    doorHintVisible = true;

    doorHintTimer = 0.f;

    doorHintAlpha = 255.f;  // 🔥 直接显示，不做出现动画
}

void TextHintManager::HideDoorHint()
{
    doorHintVisible = false;
}

void TextHintManager::UpdateDoorHint(float dt)
{
    if (!doorHintVisible) return;

    doorHintTimer += dt;

    // 🟡 进入消失阶段
    if (doorHintTimer >= DOOR_HINT_DURATION)
    {
        doorHintAlpha -= DOOR_FADE_SPEED * dt;

        if (doorHintAlpha <= 0.f)
        {
            doorHintAlpha = 0.f;
            doorHintVisible = false;
        }
    }
}
