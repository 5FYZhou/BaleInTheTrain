#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <filesystem>
using namespace sf;
#include "Constants.h"

class ResourceManager{
    private:
        std::unordered_map<TextureType, std::vector<sf::Texture>> textures;
        std::unordered_map<SoundType, SoundBuffer> soundBuffers;

        std::unordered_map<TextureType, std::vector<std::string>> texturePath;
        std::unordered_map<SoundType, std::string> soundPath;
        std::filesystem::path assetRoot;

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

        std::string ResolveTexturePath(const std::string& path) const {
            std::filesystem::path relative = path;
            if (!relative.empty() && (relative.string()[0] == '/' || relative.string()[0] == '\\')) {
                relative = relative.string().substr(1);
            }
            return (assetRoot / relative).string();
        }
    public:
        ResourceManager() {
            assetRoot = FindAssetRoot();
            std::cout << "ResourceManager asset root: " << assetRoot.string() << std::endl;
            texturePath = {
                {TextureType::BACKGROUND, 
                    { }},
                {TextureType::GRID, 
                    { }},
                {TextureType::BUTTONS, { }},
                {TextureType::NUM, { }},
                {TextureType::TIMER, { }},
                {TextureType::COUNTER, { }},
                {TextureType::Player, { "/Balestand.png", "/Baleleft.png", "/Baleright.png" }},
                {TextureType::Card, { "/cardframe.png" }},
                {TextureType::Star, { "/星星.png" }},
                {TextureType::GAMEOVER, { }},
                // New textures for Bale in the Train
                {TextureType::MenuBackground, { "/开始界面.png"}},
                {TextureType::StartButton, { "/开始游戏选项.png"}},
                {TextureType::ExitButton, { "/退出游戏选项.png"}},
                {TextureType::SettingsButton, { "/设置选项.png"}},
                {TextureType::GameBackground, { "/游戏背景.png"}},
                {TextureType::StatusBox, { "/状态框.png"}},
                {TextureType::Potion1, { "/药剂1.png"}},
                {TextureType::Potion2, { "/药剂2.png"}},
                {TextureType::Potion3, { "/药剂3.png"}},
                {TextureType::Cube, { "/魔方.png"}},
                {TextureType::SettingsIcon, { "/设置.png"}},
                {TextureType::Title, { "/标题.png"}}
            };
            soundPath = {
                {SoundType::SOUND_EXPLODE, "/bomb.wav"},
                {SoundType::SOUND_WIN, "/pass.wav"}
            };

            loadAllResources();
        }
        
        bool loadTexture(TextureType type, const std::string& path){
            Texture texture;
            if (!texture.loadFromFile(path)) {
                std::cout << "Failed to load texture: " << path << std::endl;
                return false;
            }
            textures[type].push_back(texture);
            return true;
        }

        bool loadSound(SoundType type, const std::string& path){
            SoundBuffer soundBuffer;
            if (!soundBuffer.loadFromFile(path)) {
                std::cout << "Failed to load sound: " << path << std::endl;
                return false;
            }
            soundBuffers[type] = soundBuffer;
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

        const Texture& getTexture(TextureType type, size_t index = 0) const {
            auto found = textures.find(type);
            if (found == textures.end() || index >= found->second.size()) {
                std::cerr << "ResourceManager::getTexture missing texture for type "
                          << static_cast<int>(type) << " index " << index << std::endl;
                static Texture emptyTexture;
                return emptyTexture;
            }
            return found->second[index];
        }

        const SoundBuffer& getSound(SoundType type) const {
            auto found = soundBuffers.find(type);
            if (found == soundBuffers.end()) {
                std::cerr << "ResourceManager::getSound missing sound for type "
                          << static_cast<int>(type) << std::endl;
                static SoundBuffer emptySound;
                return emptySound;
            }
            return found->second;
        }

        const int getTextureCount(TextureType type) const {
            auto found = textures.find(type);
            return found == textures.end() ? 0 : static_cast<int>(found->second.size());
        }
    };