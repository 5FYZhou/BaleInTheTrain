#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>

// Texture types for resource management
enum class TextureType { 
    BACKGROUND, GRID, BUTTONS, NUM, TIMER, COUNTER, GAMEOVER,
    Player, Monster, Key, Card,
    MenuBackground, StartButton, ExitButton, SettingsButton, GameBackground,
    StatusBox, Potion1, Potion2, Potion3,
    Cube, SettingsIcon, Title
};

enum class SoundType { SOUND_EXPLODE, SOUND_WIN };

// Game states
enum class GameState { MENU, GAME, SETTINGS, GAMEOVER, RUNNING, PAUSED, EXIT };

// Scene/Carriage system
enum class SceneType { Menu, Game, Settings };

// Player animation frames
enum class PlayerFrame { Stand, StepLeft, StepRight };

// Fade transition effect
enum class FadeState { None, FadeOut, FadeIn };


//inline constexpr int WIDTH = 9;
//inline constexpr int HEIGHT = 9;
//inline constexpr int GRIDSIZE = 25;
inline constexpr int CARD_WIDTH = 60;
inline constexpr int CARD_HEIGHT = 36;
//inline constexpr int INFOSETOFF = 5;
inline const std::string SAVE_FILE_NAME = "data/GameData.txt";
inline const std::string DATA_TEXTURE_FILE_PATH = "./assets";
inline const std::string DATA_AUDIO_FILE_PATH = "./data/Audios";

struct GameData {
    // 这里可以添加游戏数据成员，例如：
    int score;
    int level;
    // ...其他游戏数据成员...
};