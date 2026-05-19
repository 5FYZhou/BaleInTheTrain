#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
using namespace sf;
#include "Constants.h"

class ResourceManager{
    private:
        std::unordered_map<TextureType, std::vector<sf::Texture>> textures;
        std::unordered_map<SoundType, SoundBuffer> soundBuffers;

        std::unordered_map<TextureType, std::vector<std::string>> texturePath;
        std::unordered_map<SoundType, std::string> soundPath;
    public:
        ResourceManager() {
            texturePath = {
                {TextureType::BACKGROUND, 
                    { "/BK01.jpg", "/BK02.jpg", "/BK03.jpg", "/BK04.jpg", "/BK05.jpg", "/BK06.jpg", "/BK07.jpg"}},
                {TextureType::GRID, 
                    { "/Game1.jpg", "/Game2.jpg", "/Game3.jpg", "/Game4.jpg", "/Game5.jpg", "/Game6.jpg"}},
                {TextureType::BUTTONS, { "/button.jpg"}},
                {TextureType::NUM, { "/num.jpg"}},
                {TextureType::TIMER, { "/jishiqi.jpg"}},
                {TextureType::COUNTER, { "/jishuqi.jpg"}},
                {TextureType::GAMEOVER, { "/gameover.jpg"}}
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
            for (const auto& [type, paths] : texturePath) {
                for (const auto& path : paths) {
                    if (!loadTexture(type, DATA_TEXTURE_FILE_PATH + path)) {
                        return false;
                    }
                }
            }
            for (const auto& [type, path] : soundPath) {
                if (!loadSound(type, DATA_AUDIO_FILE_PATH + path)) {
                    return false;
                }
            }
            return true;
        }

        const Texture& getTexture(TextureType type, size_t index = 0) const {
            return textures.at(type)[index];
        }

        const SoundBuffer& getSound(SoundType type) const {
            return soundBuffers.at(type);
        }

        const int getTextureCount(TextureType type) const {
            return textures.at(type).size();
        }
    };