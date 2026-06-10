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

public:
    TextureType CardTypeFromKey(const std::string& cardType) const {
        if (cardType == "strike6") {
            return TextureType::Strike6;
        }
        if (cardType == "defend5") {
            return TextureType::Defend5;
        }
        return TextureType::Card;
    }

public:
    ResourceManager() {
        assetRoot = FindAssetRoot();
        audioRoot = FindAudioRoot();
        std::cout << "ResourceManager asset root: " << assetRoot.string() << std::endl;
        std::cout << "ResourceManager audio root: " << audioRoot.string() << std::endl;

        texturePath = {
            {TextureType::BACKGROUND, {}},
            {TextureType::GRID, {}},
            {TextureType::BUTTONS, {}},
            {TextureType::NUM, {}},
            {TextureType::TIMER, {}},
            {TextureType::COUNTER, {}},
            {TextureType::Player, {"/balestand1.png", "/bale1.png", "/bale2.png", "/bale3.png", "/bale4.png"}},
            {TextureType::Card, {"/cardframe.png"}},
            {TextureType::Star, {"/星星.png"}},
            {TextureType::GAMEOVER, {}},

            {TextureType::MenuBackground, {"/开始界面备用.png"}},
            {TextureType::StartButton, {"/开始游戏选项.png"}},
            {TextureType::ExitButton, {"/退出游戏选项.png"}},
            {TextureType::SettingsButton, {"/设置选项.png"}},
            {TextureType::GameBackground, {"/游戏背景备用.png"}},
            {TextureType::StatusBox, {"/状态框.png"}},
            {TextureType::Potion1, {"/药剂1.png"}},
            {TextureType::Potion2, {"/药剂2.png"}},
            {TextureType::Potion3, {"/药剂3.png"}},
            {TextureType::Cube, {"/魔方.png"}},
            {TextureType::SettingsIcon, {"/设置.png"}},
            {TextureType::Title, {"/标题.png"}},

            {TextureType::Backpack, {"/背包.png"}},
            {TextureType::DiscardPile, {"/弃牌池.png"}},
            {TextureType::BackpackInterior, {"/背包内部.png"}},
            {TextureType::BackButton, {"/返回键.png"}},
            {TextureType::DialogBox, {"/对话框.png"}},
            {TextureType::SettingsPanel, {"/设置面板.png"}},
            {TextureType::CloseButton, {"/叉叉键.png"}},
            {TextureType::Strike6, {"/打击6点.png"}},
            {TextureType::Defend5, {"/防御5点.png"}}
        };

        soundEffectPath = {
            {SoundEffect::Dialog, "/对话音效.mp3"},
            {SoundEffect::Tutorial, "/教学音效.mp3"},
            {SoundEffect::Footstep, "/走路音效.mp3"},
            {SoundEffect::Back, "/返回键音效.mp3"},
            {SoundEffect::MenuButton, "/开始界面按钮音效.wav"}
        };

        loadAllResources();
        LoadAudioResources();
    }

    bool loadTexture(TextureType type, const std::string& path) {
        sf::Texture texture;
        if (!texture.loadFromFile(path)) {
            std::cout << "Failed to load texture: " << path << std::endl;
            return false;
        }

        textures[type].push_back(std::move(texture));
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

    bool loadAllResources() {
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

    bool LoadCardTexture(const std::string& cardType) {
        const TextureType type = CardTypeFromKey(cardType);
        return textures.find(type) != textures.end() && !textures[type].empty();
    }

    bool LoadDialogAssets() {
        return getTextureCount(TextureType::DialogBox) > 0;
    }

    bool LoadSettingsAssets() {
        return getTextureCount(TextureType::SettingsPanel) > 0 &&
               getTextureCount(TextureType::CloseButton) > 0;
    }

    bool LoadAudioResources() {
        bool success = true;
        for (const auto& [effect, path] : soundEffectPath) {
            if (soundEffectBuffers.find(effect) == soundEffectBuffers.end()) {
                success &= loadSound(effect, path);
            }
        }
        return success;
    }

    const sf::Texture& getTexture(TextureType type, size_t index = 0) const {
        auto found = textures.find(type);
        if (found == textures.end() || index >= found->second.size()) {
            std::cerr << "ResourceManager::getTexture missing texture for type "
                      << static_cast<int>(type) << " index " << index << std::endl;
            static sf::Texture emptyTexture;
            return emptyTexture;
        }
        return found->second[index];
    }

    const sf::Texture& GetCardTexture(const std::string& cardType) const {
        return getTexture(CardTypeFromKey(cardType));
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

    bool HasSound(SoundEffect effect) const {
        return soundEffectBuffers.find(effect) != soundEffectBuffers.end();
    }

    int getTextureCount(TextureType type) const {
        auto found = textures.find(type);
        return found == textures.end() ? 0 : static_cast<int>(found->second.size());
    }
};
