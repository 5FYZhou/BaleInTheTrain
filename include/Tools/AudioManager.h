#pragma once
#include "Constants.h"
#include "ResourceManager.h"
#include <SFML/Audio.hpp>

#include <algorithm>
#include <deque>
#include <unordered_map>
#include <iostream>

class ResourceManager;

class AudioManager
{
private:
    struct ActiveSound
    {
        sf::Sound sound;
        SoundEffect effect;
        ActiveSound(const sf::Sound &s, SoundEffect e)
            : sound(s), effect(e) {}
    };

    std::deque<ActiveSound> activeSounds;

    // 背景音乐
    sf::Music backgroundMusic;
    bool isMusicPlaying = false;
    SoundEffect currentMusicEffect = SoundEffect::None;

    float masterVolume = DEFAULT_MASTER_VOLUME;
    float sfxVolume = DEFAULT_SFX_VOLUME;
    float musicVolume = DEFAULT_MUSIC_VOLUME;

    // 每种音效当前数量
    std::unordered_map<SoundEffect, int> soundCount;

    // 最大并发
    std::unordered_map<SoundEffect, int> maxConcurrentSounds = {
        {SoundEffect::Back, 2},
        {SoundEffect::CardFlip, 2},
        {SoundEffect::Dialog, 2},
        {SoundEffect::Footstep, 2},
        {SoundEffect::MenuButton, 2},
        {SoundEffect::Tutorial, 2},
        {SoundEffect::ObjectError, 2},
        {SoundEffect::Pickup, 2},
        {SoundEffect::PlayerAttack, 1}};

public:
    ResourceManager *rm = nullptr;

    AudioManager() = default;

    ~AudioManager()
    {
        StopAllSounds();
    }

    // =========================
    // 初始化
    // =========================
    inline bool Initialize(ResourceManager &resourceMgr)
    {
        rm = &resourceMgr;
        activeSounds.clear();
        soundCount.clear();
        return true;
    }

    // =========================
    // 更新（清理结束音效）
    // =========================
    inline void Update()
    {
        activeSounds.erase(
            std::remove_if(
                activeSounds.begin(),
                activeSounds.end(),
                [this](ActiveSound &s)
                {
                    if (s.sound.getStatus() == sf::SoundSource::Status::Stopped)
                    {
                        auto it = soundCount.find(s.effect);
                        if (it != soundCount.end() && it->second > 0)
                            it->second--;

                        return true;
                    }
                    return false;
                }),
            activeSounds.end());
    }

    // =========================
    // 播放音效
    // =========================
    inline void PlaySound(SoundEffect effect, float volumeScale = 1.0f)
    {
        if (!rm || effect == SoundEffect::None || !rm->HasSound(effect))
            return;

        int currentCount = soundCount[effect];
        int maxCount = maxConcurrentSounds.count(effect) ? maxConcurrentSounds[effect] : 3;

        if (currentCount >= maxCount)
            return;

        Update();

        activeSounds.emplace_back(
            ActiveSound{
                sf::Sound(rm->getSound(effect)),
                effect});

        auto &sound = activeSounds.back().sound;

        ApplyVolumeToSound(sound, sfxVolume, volumeScale);
        sound.play();

        soundCount[effect]++;
    }

    // =========================
    // 停止所有音效
    // =========================
    inline void StopAllSounds()
    {
        for (auto &s : activeSounds)
            s.sound.stop();

        activeSounds.clear();
        soundCount.clear();
    }

    // =========================
    // 音乐控制
    // =========================
    inline bool PlayMusic(SoundEffect effect, float volumeScale = 1.0f)
    {
        if (!rm || effect == SoundEffect::None)
            return false;

        if (currentMusicEffect == effect && isMusicPlaying)
            return true;

        std::string path = rm->GetSoundPath(effect);
        if (path.empty())
            return false;

        StopMusic();

        if (!backgroundMusic.openFromFile(path))
            return false;

        backgroundMusic.setLooping(true);

        float vol = 100.f * masterVolume * musicVolume * volumeScale;
        backgroundMusic.setVolume(std::clamp(vol, 0.f, 100.f));

        backgroundMusic.play();

        isMusicPlaying = true;
        currentMusicEffect = effect;

        return true;
    }

    inline void StopMusic()
    {
        backgroundMusic.stop();
        isMusicPlaying = false;
        currentMusicEffect = SoundEffect::None;
    }

    inline void PauseMusic()
    {
        if (isMusicPlaying)
            backgroundMusic.pause();
    }

    inline void ResumeMusic()
    {
        if (backgroundMusic.getStatus() == sf::SoundSource::Status::Paused)
        {
            backgroundMusic.play();
            isMusicPlaying = true;
        }
    }

    // =========================
    // 音量
    // =========================
    inline void SetMasterVolume(float v)
    {
        masterVolume = std::clamp(v, 0.f, 1.f);
    }

    inline void SetSfxVolume(float v)
    {
        sfxVolume = std::clamp(v, 0.f, 1.f);
    }

    inline void SetMusicVolume(float v)
    {
        musicVolume = std::clamp(v, 0.f, 1.f);
    }

    inline float GetMasterVolume() const { return masterVolume; }
    inline float GetSfxVolume() const { return sfxVolume; }
    inline float GetMusicVolume() const { return musicVolume; }

private:
    inline void ApplyVolumeToSound(sf::Sound &sound, float base, float scale)
    {
        float v = 100.f * masterVolume * base * scale;
        sound.setVolume(std::clamp(v, 0.f, 100.f));
    }
};

// =========================
// 全局声明（用 extern）
// =========================
extern AudioManager GlobalaudioMgr;