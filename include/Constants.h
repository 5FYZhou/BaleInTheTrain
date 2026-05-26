#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>

enum class TextureType { BACKGROUND, GRID, BUTTONS, NUM, TIMER, COUNTER, GAMEOVER,
    Player, Monster, Key, Card };
enum class SoundType { SOUND_EXPLODE, SOUND_WIN };

enum class GameState { MENU, GAMEOVER, RUNNING, PAUSED, EXIT };


//inline constexpr int WIDTH = 9;
//inline constexpr int HEIGHT = 9;
//inline constexpr int GRIDSIZE = 25;
inline constexpr int CARD_WIDTH = 60;
inline constexpr int CARD_HEIGHT = 36;
//inline constexpr int INFOSETOFF = 5;
inline const std::string SAVE_FILE_NAME = "data/GameData.txt";
inline const std::string DATA_TEXTURE_FILE_PATH = "./data/images";
inline const std::string DATA_AUDIO_FILE_PATH = "./data/Audios";

struct GameData {
    // 这里可以添加游戏数据成员，例如：
    int score;
    int level;
    // ...其他游戏数据成员...
};