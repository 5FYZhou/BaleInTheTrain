#pragma once

#include "Constants.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class ResourceManager {
private:
    std::unordered_map<TextureType, std::vector<sf::Texture>> textures;
    std::unordered_map<SoundEffect, sf::SoundBuffer> soundEffectBuffers;

    std::unordered_map<TextureType, std::vector<std::string>> texturePath;
    std::unordered_map<SoundEffect, std::string> soundEffectPath;
    std::filesystem::path assetRoot;
    std::filesystem::path audioRoot;

    sf::Font font;

    std::filesystem::path FindAssetRoot() const {
        const std::vector<std::filesystem::path> candidates = {
            std::filesystem::current_path(),
            std::filesystem::current_path() / "..",
            std::filesystem::current_path() / ".." / "..",
            std::filesystem::current_path() / ".." / ".." / ".."
        };

        for (const auto& candidate : candidates) {
            const auto path = std::filesystem::weakly_canonical(candidate / "assets");
            if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
                return path;
            }
        }

        return std::filesystem::path(DATA_TEXTURE_FILE_PATH);
    }

    std::filesystem::path FindAudioRoot() const {
        const std::vector<std::filesystem::path> candidates = {
            std::filesystem::current_path() / "audio",
            std::filesystem::current_path() / ".." / "audio",
            std::filesystem::current_path() / ".." / ".." / "audio",
            std::filesystem::current_path() / ".." / ".." / ".." / "audio"
        };

        for (const auto& candidate : candidates) {
            const auto path = std::filesystem::weakly_canonical(candidate);
            if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
                return path;
            }
        }

        return std::filesystem::path(DATA_AUDIO_FILE_PATH);
    }

    std::string ResolveTexturePath(const std::string& path) const {
        std::filesystem::path relative = path;
        const std::string raw = relative.string();
        if (!raw.empty() && (raw[0] == '/' || raw[0] == '\\')) {
            relative = raw.substr(1);
        }
        return (assetRoot / relative).string();
    }

    std::string ResolveAudioPath(const std::string& path) const {
        std::filesystem::path relative = path;
        const std::string raw = relative.string();
        if (!raw.empty() && (raw[0] == '/' || raw[0] == '\\')) {
            relative = raw.substr(1);
        }
        return (audioRoot / relative).string();
    }


    bool loadTextureFromUnicodePath(sf::Texture& texture, const std::string& utf8Path) {
#ifdef _WIN32
            std::filesystem::path fsPath(utf8Path);
            std::wstring widePath = fsPath.wstring();
            
            FILE* file = _wfopen(widePath.c_str(), L"rb");
            if (!file) {
                return false;
            }
            
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            std::vector<unsigned char> buffer(fileSize);
            size_t bytesRead = fread(buffer.data(), 1, fileSize, file);
            fclose(file);
            
            if (bytesRead != fileSize) {
                return false;
            }
            
            return texture.loadFromMemory(buffer.data(), buffer.size());
#else
            return texture.loadFromFile(utf8Path);
#endif
        }

public:
    ResourceManager() {
        assetRoot = FindAssetRoot();
        audioRoot = FindAudioRoot();
        std::cout << "ResourceManager asset root: " << assetRoot.string() << std::endl;
        std::cout << "ResourceManager audio root: " << audioRoot.string() << std::endl;

        texturePath = {
            // 开始界面
            {TextureType::MenuBackground, {"/开始界面备用.png"}},
            {TextureType::Title, {"/标题.png"}},
            {TextureType::StartButton, {"/开始游戏选项.png"}},
            {TextureType::SettingsButton, {"/设置选项.png"}},
            {TextureType::ExitButton, {"/退出游戏选项.png"}},
            {TextureType::Win, {"/逃离成功.png"}},

            // 游戏界面
            {TextureType::GameBackground, {"/游戏背景备用.png"}},
            {TextureType::Player, {"/balestand1.png", "/bale1.png", "/bale2.png", "/bale3.png", "/bale4.png"}},
            {TextureType::Star, {"/星星.png"}},
            {TextureType::Key, {"/钥匙.png"}},
            // 游戏UI
            {TextureType::StatusBox, {"/状态框.png"}},
            {TextureType::Potion1, {"/药剂1.png"}},
            {TextureType::Potion2, {"/药剂2.png"}},
            {TextureType::Potion3, {"/药剂3.png"}},
            {TextureType::Cube, {"/魔方.png"}},
            {TextureType::SettingsIcon, {"/设置.png"}},
            {TextureType::NotificationBG, {"/获得物品框.png"}},

            {TextureType::BackpackIcon, {"/背包.png"}},
            {TextureType::DiscardPile, {"/弃牌池.png"}},
            {TextureType::BackpackInterior, {"/背包内部.png"}},
            {TextureType::BackButton, {"/返回键.png"}},
            {TextureType::DialogBox, {"/对话框.png"}},
            {TextureType::SettingsPanel, {"/设置面板.png"}},
            {TextureType::CloseButton, {"/叉叉键.png"}},
            {TextureType::EndTurn, {"/结束回合.png"}},

            // 卡牌
            {TextureType::Strike, {"/打击6点.png"}},
            {TextureType::Defend, {"/防御5点.png"}},
            {TextureType::Rage, {"/暴走.png"}},
            {TextureType::Shrug_off, {"/耸肩无视.png"}},
            {TextureType::Heavy_strike, {"/痛击.png"}},
            {TextureType::Anger, {"/愤怒.png"}},
            {TextureType::Continuous_punches, {"/连续拳.png"}},
            {TextureType::Observe_weaknesses, {"/观察弱点.png"}},
            {TextureType::Activate_muscles, {"/活动肌肉.png"}},
            {TextureType::Revitalize_spirit, {"/重振精神.png"}},
            {TextureType::Metallization, {"/金属化.png"}},
            {TextureType::Unstoppable, {"/势不可当.png"}},
            {TextureType::Rampart, {"/壁垒.png"}},
            {TextureType::Sacrifice, {"/祭品.png"}},

            // 敌人
            {TextureType::Train_attendant, {"/乘务员1.png", "/乘务员2.png"}},
            {TextureType::LightMonster, {"/车灯1.png", "/车灯2.png"}},
            {TextureType::TicketMonster, {"/车票1.png", "/车票2.png"}},
            {TextureType::TyreMosnter, {"/车轮1.png", "/车轮2.png"}},

            //敌人意图
            {TextureType::PlanAttack, {"/敌方攻击.png"}},
            {TextureType::PlanDefend, {"/敌方防御.png"}}
            
        };

        soundEffectPath = {
            {SoundEffect::Dialog, "/对话音效.mp3"},
            {SoundEffect::Tutorial, "/教学音效.mp3"},
            {SoundEffect::Footstep, "/走路音效.mp3"},
            {SoundEffect::Back, "/返回键音效.mp3"},
            {SoundEffect::MenuButton, "/开始界面按钮音效.wav"}
        };

        loadAllTex();
        LoadAllAudio();
        LoadFont();
    }

     bool loadTexture(TextureType type, const std::string& path){
            sf::Texture texture;
            bool success = false;
            
#ifdef _WIN32
            success = loadTextureFromUnicodePath(texture, path);
#else
            success = texture.loadFromFile(path);
#endif
            
            if (!success) {
                std::cout << "Failed to load texture: " << path << std::endl;
                return false;
            }
            textures[type].push_back(texture);
            return true;
        }


    bool loadSound(SoundEffect effect, const std::string& path) {
        if (effect == SoundEffect::None) {
            return false;
        }

        sf::SoundBuffer soundBuffer;
        const std::string resolved = ResolveAudioPath(path);
        if (!soundBuffer.loadFromFile(resolved)) {
            std::cout << "Failed to load sound effect: " << resolved << std::endl;
            return false;
        }

        soundEffectBuffers[effect] = std::move(soundBuffer);
        return true;
    }

    bool loadAllTex() {
        bool success = true;
        for (const auto& [type, paths] : texturePath) {
            for (const auto& path : paths) {
                const std::string resolved = ResolveTexturePath(path);
                if (!loadTexture(type, resolved)) {
                    success = false;
                }
            }
        }
        return success;
    }

    bool LoadDialogAssets() {
        return getTextureCount(TextureType::DialogBox) > 0;
    }

    bool LoadSettingsAssets() {
        return getTextureCount(TextureType::SettingsPanel) > 0 &&
               getTextureCount(TextureType::CloseButton) > 0;
    }

    bool LoadAllAudio() {
        bool success = true;
        for (const auto& [effect, path] : soundEffectPath) {
            if (soundEffectBuffers.find(effect) == soundEffectBuffers.end()) {
                success &= loadSound(effect, path);
            }
        }
        return success;
    }

    bool LoadFont(){
        const std::array<std::string, 5> candidates = {
            "C:/Windows/Fonts/msyh.ttc",
            "C:/Windows/Fonts/simhei.ttf",
            "C:/Windows/Fonts/simsun.ttc",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf"
        };

        for (const auto& path : candidates) {
            if (font.openFromFile(path)) {
                return true;
            }
        }

        std::cout << "Failed to load system font. Text UI will be hidden.\n";
        return false;
} 

    const sf::Texture& getTexture(TextureType type, size_t index = 0) const {
        static sf::Texture emptyTexture;
        if(type == TextureType::None){
            return emptyTexture;
        }
        auto found = textures.find(type);
        if (found == textures.end() || index >= found->second.size()) {
            std::cerr << "ResourceManager::getTexture missing texture for type "
                      << static_cast<int>(type) << " index " << index << std::endl;
            return emptyTexture;
        }
        return found->second[index];
    }

    const sf::Texture& GetSettingsPanelTexture() const {
        return getTexture(TextureType::SettingsPanel);
    }

    const sf::Texture& GetDialogBoxTexture() const {
        return getTexture(TextureType::DialogBox);
    }

    const sf::SoundBuffer& getSound(SoundEffect effect) const {
        auto found = soundEffectBuffers.find(effect);
        if (found == soundEffectBuffers.end()) {
            std::cerr << "ResourceManager::getSound missing sound effect for type "
                      << static_cast<int>(effect) << std::endl;
            static sf::SoundBuffer emptySound;
            return emptySound;
        }
        return found->second;
    }

    const sf::Font& getFont() const { return font; }

    bool HasSound(SoundEffect effect) const {
        return soundEffectBuffers.find(effect) != soundEffectBuffers.end();
    }

    int getTextureCount(TextureType type) const {
        auto found = textures.find(type);
        return found == textures.end() ? 0 : static_cast<int>(found->second.size());
    }
};
