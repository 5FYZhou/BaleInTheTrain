#include "TextHintManager.h"
#include "AudioManager.h"

#include <algorithm>

namespace {
std::vector<std::string> DefaultIntroDialog() {
    return {
        "Bale:\n……这是哪？刚才那阵撞击之后，整节车厢都安静得不正常。",
        "Bale:\n怎么没有人？大家都去哪里了？车子停下来了，试试打开门。",
        "Bale:\n门被锁住了。想离开这里，必须找到钥匙，或者能拼成钥匙的碎片。",
        "Bale:\n这里有个背包。里面放着5张“打击6点”和5张“防御5点”的卡牌。",
        "Bale:\n这些牌也许能在战斗里派上用场。先收好。接下来，去别的车厢看看。"
    };
}
}
const std::string TextHintManager::DOOR_HINT_TEXT = "一扇有三把锁的门";

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
