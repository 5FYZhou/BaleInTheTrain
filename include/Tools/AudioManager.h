#pragma once
#include "Constants.h"
#include "ResourceManager.h"
#include <SFML/Audio.hpp>

#include <algorithm>
#include <deque>
#include <unordered_map>

class ResourceManager;

class AudioManager
{
private:


    std::deque<sf::Sound> activeSounds;

    //背景音乐
    sf::Music backgroundMusic;
    bool isMusicPlaying = false;
    SoundEffect currentMusicEffect = SoundEffect::None;

    float masterVolume = DEFAULT_MASTER_VOLUME;
    float sfxVolume = DEFAULT_SFX_VOLUME;
    float musicVolume = DEFAULT_MUSIC_VOLUME;

    // 每种音效类型当前正在播放的数量
    std::unordered_map<SoundEffect, int> soundCount;

    // 每种音效的最大同时播放数量
    std::unordered_map<SoundEffect, int> maxConcurrentSounds = {
        {SoundEffect::Back, 2},
        {SoundEffect::CardFlip, 2},
        // { SoundEffect::Defeat, 2 },
        {SoundEffect::Dialog, 2},
        {SoundEffect::Footstep, 2},
        {SoundEffect::MenuButton, 2},
        {SoundEffect::Tutorial, 2},
        //{ SoundEffect::Victory, 2 },
        {SoundEffect::ObjectError, 10},
        {SoundEffect::Pickup, 30},
        {SoundEffect::PlayerAttack, 100},
        {SoundEffect::BUFFandDEBUFF, 50},
    };

public:
    ResourceManager *rm = nullptr;
    
    // 播放背景音乐（使用 SoundEffect 枚举）
    inline bool PlayMusic(SoundEffect effect, float volumeScale = 1.0f){
        if(effect == SoundEffect::None || !rm)
            return false;

        // 如果已经播放同一首歌，不重复加载
        if(currentMusicEffect == effect && isMusicPlaying){
            return true;
        }

        // 通过 ResourceManager 获取音乐文件路径
        std::string musicPath = rm->GetSoundPath(effect);
        if(musicPath.empty()){
            std::cerr << "AudioManager: No path found for music effect " << static_cast<int>(effect) << std::endl;
            return false;
        }

        // 停止当前音乐
        StopMusic();

        // 加载并播放新音乐
        if(!backgroundMusic.openFromFile(musicPath)){
            std::cerr << "AudioManager: Failed to load music from " << musicPath << std::endl;
            return false;
        }

        backgroundMusic.setLooping(true);
        float finalVolume = 100.f * masterVolume * musicVolume * volumeScale;
        finalVolume = std::clamp(finalVolume, 0.0f, 100.0f);
        backgroundMusic.setVolume(finalVolume);
        backgroundMusic.play();
        
        isMusicPlaying = true;
        currentMusicEffect = effect;
        return true;
    }

    inline void StopMusic(){
        if(backgroundMusic.getStatus() != sf::SoundSource::Status::Stopped){
            backgroundMusic.stop();
        }
        isMusicPlaying = false;
        currentMusicEffect = SoundEffect::None;
    }

    inline void PauseMusic(){
        if(isMusicPlaying){
            backgroundMusic.pause();
        }
    }

    inline void ResumeMusic(){
        if(!isMusicPlaying && backgroundMusic.getStatus() == sf::SoundSource::Status::Paused){
            backgroundMusic.play();
            isMusicPlaying = true;
        }
    }

    inline bool IsMusicPlaying() const { 
        return isMusicPlaying || backgroundMusic.getStatus() == sf::SoundSource::Status::Playing;
    }

    inline void UpdateMusicVolume(){
        if(isMusicPlaying || backgroundMusic.getStatus() != sf::SoundSource::Status::Stopped){
            float finalVolume = 100.f * masterVolume * musicVolume;
            finalVolume = std::clamp(finalVolume, 0.0f, 100.0f);
            backgroundMusic.setVolume(finalVolume);
        }
    }

    AudioManager() = default;

    ~AudioManager()
    {
        StopAllSounds();
    }

    inline bool Initialize(ResourceManager &resourceMgr)
    {
        rm = &resourceMgr;
        activeSounds.clear();
        soundCount.clear();
        return true;
    }

    inline void Update()
    {
        static int frameCounter = 0;
        frameCounter++;

        if (frameCounter % 10 != 0)
            return;
        // 清理已停止的声音
        activeSounds.erase(
            std::remove_if(
                activeSounds.begin(), activeSounds.end(),
                [this](const sf::Sound &sound)
                {
                    if (sound.getStatus() == sf::SoundSource::Status::Stopped)
                    {
                        // 找到对应音效类型并减少计数
                        for (auto &[effect, count] : soundCount)
                        {
                            if (count > 0)
                            {
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

    inline void PlaySound(SoundEffect effect, float volumeScale = 1.0f)
    {
        //std::cout << "this = " << this << std::endl;
        //std::cout << "rm   = " << rm << std::endl;
       // if(rm == nullptr)  std::cout <<"!rm" << std::endl;
        //std::cout << "HasSound: " << rm->HasSound(effect) << std::endl;
       // std::cout << "1PlaySound called, effect: " << static_cast<int>(effect) << std::endl;
        if (effect == SoundEffect::None || !rm || !rm->HasSound(effect))
            return;

       // std::cout << "2PlaySound called, effect: " << static_cast<int>(effect) << std::endl;

        // 检查是否达到上限
        int currentCount = soundCount[effect];
        int maxCount = maxConcurrentSounds.count(effect) ? maxConcurrentSounds[effect] : 3; // 默认上限3
        if (currentCount >= maxCount)
            return;

       // std::cout << "3PlaySound called, effect: " << static_cast<int>(effect) << std::endl;

        // 顺便清理已停止音效
        Update();

        // 播放音效
        activeSounds.emplace_back(rm->getSound(effect));
        sf::Sound &sound = activeSounds.back();

        ApplyVolumeToSound(sound, sfxVolume, volumeScale);

        sound.play();

        // 增加计数
        soundCount[effect]++;

        //std::cout << "4PlaySound called, effect: " << static_cast<int>(effect) << std::endl;
    }

    inline void StopAllSounds()
    {
        for (auto &sound : activeSounds)
        {
            sound.stop();
        }
        activeSounds.clear();
        soundCount.clear();
    }

    inline void SetMasterVolume(float volume)
    {
        masterVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    inline void SetSfxVolume(float volume)
    {
        sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    inline void SetMusicVolume(float volume)
    {
        musicVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    inline float GetMasterVolume() const { return masterVolume; }
    inline float GetSfxVolume() const { return sfxVolume; }
    inline float GetMusicVolume() const { return musicVolume; }

private:
    inline void ApplyVolumeToSound(sf::Sound &sound, float baseVolume, float volumeScale)
    {
        float finalVolume = 100.f * masterVolume * baseVolume * volumeScale;
        finalVolume = std::clamp(finalVolume, 0.0f, 100.0f);
        sound.setVolume(finalVolume);
    }
};

extern AudioManager GlobalaudioMgr;