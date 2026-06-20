#pragma once
#include "Constants.h"
#include "ResourceManager.h"
#include <SFML/Audio.hpp>

#include <algorithm>
#include <deque>
#include <unordered_map>

class ResourceManager;

class AudioManager {
private:
    ResourceManager* rm = nullptr;

    std::deque<sf::Sound> activeSounds;

    float masterVolume = DEFAULT_MASTER_VOLUME;
    float sfxVolume = DEFAULT_SFX_VOLUME;
    float musicVolume = DEFAULT_MUSIC_VOLUME;

    // 每种音效类型当前正在播放的数量
    std::unordered_map<SoundEffect, int> soundCount;

    // 每种音效的最大同时播放数量
    std::unordered_map<SoundEffect, int> maxConcurrentSounds = {
        { SoundEffect::Back, 2 },
        { SoundEffect::CardFlip, 2 },
        { SoundEffect::Defeat, 2 },
        { SoundEffect::Dialog, 2 },
        { SoundEffect::Footstep, 2 },
        { SoundEffect::MenuButton, 2 },
        { SoundEffect::Tutorial, 2 },
        { SoundEffect::Victory, 2 },
    };

public:
    AudioManager() = default;

    ~AudioManager(){
        StopAllSounds();
    }

    inline bool Initialize(ResourceManager& resourceMgr){
        rm = &resourceMgr;
        activeSounds.clear();
        soundCount.clear();
        return true;
    }

    inline void Update(){
        static int frameCounter = 0;
    frameCounter++;

    if(frameCounter % 10 != 0)
        return;
        // 清理已停止的声音
        activeSounds.erase(
            std::remove_if(
                activeSounds.begin(), activeSounds.end(),
                [this](const sf::Sound& sound)
                {
                    if(sound.getStatus() == sf::SoundSource::Status::Stopped){
                        // 找到对应音效类型并减少计数
                        for(auto& [effect, count] : soundCount){
                            if(count > 0){
                                --count;
                                break; // 假设每个声音只对应一个类型
                            }
                        }
                        return true;
                    }
                    return false;
                }),
            activeSounds.end());
    }

    inline void PlaySound(SoundEffect effect, float volumeScale = 1.0f){
        if(effect == SoundEffect::None || !rm || !rm->HasSound(effect))
            return;

        // 检查是否达到上限
        int currentCount = soundCount[effect];
        int maxCount = maxConcurrentSounds.count(effect) ? maxConcurrentSounds[effect] : 3; // 默认上限3
        if(currentCount >= maxCount)
            return;

        // 顺便清理已停止音效
        Update();

        // 播放音效
        activeSounds.emplace_back(rm->getSound(effect));
        sf::Sound& sound = activeSounds.back();

        ApplyVolumeToSound(sound, sfxVolume, volumeScale);

        sound.play();

        // 增加计数
        soundCount[effect]++;
    }

    inline void StopAllSounds(){
        for(auto& sound : activeSounds){
            sound.stop();
        }
        activeSounds.clear();
        soundCount.clear();
    }

    inline void SetMasterVolume(float volume){
        masterVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    inline void SetSfxVolume(float volume){
        sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    inline void SetMusicVolume(float volume){
        musicVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    inline float GetMasterVolume() const { return masterVolume; }
    inline float GetSfxVolume() const { return sfxVolume; }
    inline float GetMusicVolume() const { return musicVolume; }

private:
    inline void ApplyVolumeToSound(sf::Sound& sound, float baseVolume, float volumeScale){
        float finalVolume = 100.f * masterVolume * baseVolume * volumeScale;
        finalVolume = std::clamp(finalVolume, 0.0f, 100.0f);
        sound.setVolume(finalVolume);
    }
};