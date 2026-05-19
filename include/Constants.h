#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics.hpp>

enum class TextureType { BACKGROUND, GRID, BUTTONS, NUM, TIMER, COUNTER, GAMEOVER };
enum class SoundType { SOUND_EXPLODE, SOUND_WIN };

// GridType is used to determine the character and color to display for each cell in the scene
enum class GridType { EMPTY, DEFAULT, MINE, 
    ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT,
    FLAG, QUESTION, BACKUP, BOMB, UNFUNDMINE };
enum class GameState { MENU, GAMEOVER, RUNNING, PAUSED, EXIT };


inline constexpr int WIDTH = 9;
inline constexpr int HEIGHT = 9;
inline constexpr int GRIDSIZE = 25;
inline constexpr int BTN_WIDTH = 60;
inline constexpr int BTN_HEIGHT = 36;
inline constexpr int INFOSETOFF = 5;
inline const std::string SAVE_FILE_NAME = "data/GameData.txt";
inline const std::string DATA_TEXTURE_FILE_PATH = "./data/images";
inline const std::string DATA_AUDIO_FILE_PATH = "./data/Audios";


struct Difficulty {
    int width, height;
    int mineNum;
    Difficulty(int w, int h, int m): width(w), height(h), mineNum(m){}
};

enum class Diff { EASY, MIDDLE, DIFFICULT };

inline const std::unordered_map<Diff, Difficulty> DiffMap = {
    { Diff::EASY, { 9, 9, 10 }},
    { Diff::MIDDLE, { 16, 16, 40 }},
    { Diff::DIFFICULT, { 30, 16, 99 }}
};


enum class ButtonState {
    Default,
    Hover,
    Pressed
};

enum class ButtonType {
    Easy = 0,
    Normal,
    Hard,
    BG,
    Skin,
    Restart,
    Quit
};

struct Button_my {
    ButtonType type;
    sf::IntRect rect;
    ButtonState state;
    Button_my(ButtonType t) : type(t), rect({0, 0}, {0, 0}), state(ButtonState::Default) {}
    Button_my(ButtonType t, sf::IntRect r) : type(t), rect(r), state(ButtonState::Default) {}
};