#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// Texture types for resource management.
enum class TextureType {
    None, 
    MenuBackground, Title, StartButton, ExitButton, SettingsButton,
    GameBackground, StatusBox, Potion1, Potion2, Potion3, Cube,
    Player, Star, Key,
    SettingsIcon, SettingsPanel, 
    BackpackIcon, DiscardPile, BackpackInterior, BackButton, DialogBox,
    CloseButton, EndTurn, Win, 

    Strike, //打击24
    Defend, //防御1
    Rage, //暴走2
    Shrug_off, //耸肩无视3
    Heavy_strike, //痛击4
    Anger, //愤怒5
    Continuous_punches, //连续拳6
    Observe_weaknesses, //观察弱点7
    Activate_muscles, //活动肌肉8
    Revitalize_spirit, //重振精神9
    Metallization, //金属化10
    Unstoppable, //势不可挡11
    Rampart, //壁垒12
    Sacrifice, //祭品13
    
    Train_attendant, //列车员
    LightMonster, //车灯怪
    TicketMonster, //车票怪
    TyreMosnter, //轮胎怪
};

// Sound/audio types for resource management.
enum class SoundEffect {
    Dialog,
    Tutorial,
    Footstep,
    Back,
    MenuButton,
    CardFlip,
    Victory,
    Defeat,
    None
};

// Game states.
enum class GameState { MENU, GAME, SETTINGS, GAMEOVER, RUNNING, PAUSED, EXIT };

// Scene/carriage system.
enum class SceneType { Menu, Game, Battle, Win };

enum class ItemType {
    Button,
    Strike, //打击0
    Defend, //防御1
    Rage, //暴走2
    Shrug_off, //耸肩无视3
    Heavy_strike, //痛击4
    Anger, //愤怒5
    Continuous_punches, //连续拳6
    Observe_weaknesses, //观察弱点7
    Activate_muscles, //活动肌肉8
    Revitalize_spirit, //重振精神9
    Metallization, //金属化10
    Unstoppable, //势不可挡11
    Rampart, //壁垒12
    Sacrifice, //祭品13
    Key, // 钥匙
    Potion1, // 药剂1
    Potion2,
    Potion3,
    Player, 
    Enemy
};

enum class EventType {
    None,
    StartGame, OpenSettings, ExitGame, ReturnMenu,
    OpenBackpackIcon, OpenDiscardPile, OpenDealCardPanel, 
    MusicVolumeChange, SfxVolumeChange, 
    ResetPlayerPos,
    ItemClicked, 
    GenericAction,
    BeginBattle, EndTurn, EndBattle,
    Win, KeysInsufficient,
};

struct GameEvent {
    EventType type = EventType::None;
    float val = -1.f;
};

enum class StatusType{
    None,
    easy_to_attack, //易伤（受到的伤害增加）
    power_up, //力量
    rampart, //壁垒（回合开始时防御点数不消失）
    vulnerable, //虚弱（攻击造成的伤害降低）
    thorns, //反伤（受到攻击时对攻击者造成伤害）
};

enum class EnemyType{
   Train_attendant, //列车员
    LightMonster, //车灯怪
    TicketMonster, //车票怪
    TyreMosnter, //轮胎怪
};

enum class PileType{
    Strike, //打击0
    Defend, //防御1
    Rage, //暴走2
    Shrug_off, //耸肩无视3
    Heavy_strike, //痛击4
    Anger, //愤怒5
    Continuous_punches, //连续拳6
    Observe_weaknesses, //观察弱点7
    Activate_muscles, //活动肌肉8
    Revitalize_spirit, //重振精神9
    Metallization, //金属化10
    Unstoppable, //势不可挡11
    Rampart, //壁垒12
    Sacrifice //祭品13
};

// 卡牌类型映射纹理类型
inline const std::unordered_map<PileType, TextureType> cardTexMap = {
    { PileType::Strike, TextureType::Strike },
    { PileType::Defend, TextureType::Defend },
    { PileType::Rage, TextureType::Rage },
    { PileType::Shrug_off, TextureType::Shrug_off },
    { PileType::Heavy_strike, TextureType::Heavy_strike },
    { PileType::Anger, TextureType::Anger },
    { PileType::Continuous_punches, TextureType::Continuous_punches },
    { PileType::Observe_weaknesses, TextureType::Observe_weaknesses },
    { PileType::Activate_muscles, TextureType::Activate_muscles },
    { PileType::Revitalize_spirit, TextureType::Revitalize_spirit },
    { PileType::Metallization, TextureType::Metallization },
    { PileType::Unstoppable, TextureType::Unstoppable },
    { PileType::Rampart, TextureType::Rampart },
    { PileType::Sacrifice, TextureType::Sacrifice }
};

// 敌人类型映射纹理类型
inline const std::unordered_map<EnemyType, TextureType> enemyTexMap = {
    { EnemyType::Train_attendant, TextureType::Train_attendant },
    { EnemyType::LightMonster, TextureType::LightMonster },
    { EnemyType::TicketMonster, TextureType::TicketMonster },
    { EnemyType::TyreMosnter, TextureType::TyreMosnter }
};

// 场景物品类型映射卡牌类型
inline const std::unordered_map<ItemType, PileType> itemPileMap = {
    { ItemType::Strike, PileType::Strike },
    { ItemType::Defend, PileType::Defend },
    { ItemType::Rage, PileType::Rage },
    { ItemType::Shrug_off, PileType::Shrug_off },
    { ItemType::Heavy_strike, PileType::Heavy_strike },
    { ItemType::Anger, PileType::Anger },
    { ItemType::Continuous_punches, PileType::Continuous_punches },
    { ItemType::Observe_weaknesses, PileType::Observe_weaknesses },
    { ItemType::Activate_muscles, PileType::Activate_muscles },
    { ItemType::Revitalize_spirit, PileType::Revitalize_spirit },
    { ItemType::Metallization, PileType::Metallization },
    { ItemType::Unstoppable, PileType::Unstoppable },
    { ItemType::Rampart, PileType::Rampart },
    { ItemType::Sacrifice, PileType::Sacrifice }
};

// 场景物品类型映射纹理类型
inline const std::unordered_map<ItemType, TextureType> itemTexMap = {
    { ItemType::Strike, TextureType::Strike },
    { ItemType::Defend, TextureType::Defend },
    { ItemType::Rage, TextureType::Rage },
    { ItemType::Shrug_off, TextureType::Shrug_off },
    { ItemType::Heavy_strike, TextureType::Heavy_strike },
    { ItemType::Anger, TextureType::Anger },
    { ItemType::Continuous_punches, TextureType::Continuous_punches },
    { ItemType::Observe_weaknesses, TextureType::Observe_weaknesses },
    { ItemType::Activate_muscles, TextureType::Activate_muscles },
    { ItemType::Revitalize_spirit, TextureType::Revitalize_spirit },
    { ItemType::Metallization, TextureType::Metallization },
    { ItemType::Unstoppable, TextureType::Unstoppable },
    { ItemType::Rampart, TextureType::Rampart },
    { ItemType::Sacrifice, TextureType::Sacrifice },
    { ItemType::Key, TextureType::Key },
    { ItemType::Potion1, TextureType::Potion1 },
    { ItemType::Potion2, TextureType::Potion2 },
    { ItemType::Potion3, TextureType::Potion3 }
};

struct CardView {
    PileType cardType;
    TextureType texType;    // 纹理类型
    sf::Vector2f basePosition;   // 基础位置
    float rotation = 0.f;              // 旋转角度（度）
};

inline constexpr int WINDOW_WIDTH = 1920;
inline constexpr int WINDOW_HEIGHT = 1080;
inline constexpr int CARD_WIDTH = 60;
inline constexpr int CARD_HEIGHT = 36;
inline const std::string SAVE_FILE_NAME = "data/GameData.txt";
inline const std::string DATA_TEXTURE_FILE_PATH = "./assets";
inline const std::string DATA_AUDIO_FILE_PATH = "./audios";

// Dialog and intro card reward configuration.
inline constexpr int INTRO_DIALOG_COUNT = 5;
inline constexpr int INTRO_BACKPACK_INDEX = 3;
inline constexpr int INITIAL_STRIKE_COUNT = 5;
inline constexpr int INITIAL_DEFEND_COUNT = 5;
inline constexpr int REWARD_STRIKE_COUNT = 3;
inline constexpr int REWARD_DEFEND_COUNT = 2;
inline constexpr float DIALOG_FADE_IN_SPEED = 900.f;
inline constexpr float DIALOG_FADE_OUT_SPEED = 520.f;
inline constexpr float MOVEMENT_HINT_DURATION = 5.f;

// UI configuration.
inline constexpr float POTION_ICON_GRAY = 120.f;
inline constexpr float POTION_ICON_ALPHA = 155.f;
inline constexpr float POTION_ICON_SCALE = 0.72f;

// Audio configuration.
inline constexpr float DEFAULT_MASTER_VOLUME = 1.0f;
inline constexpr float DEFAULT_SFX_VOLUME = 0.75f;
inline constexpr float DEFAULT_MUSIC_VOLUME = 0.7f;
inline constexpr float FOOTSTEP_VOLUME_SCALE = 0.65f;

// Carriage boundaries and entry/exit positions
inline const float LeftExitX = 210.f;      // Player exit on left
inline const float RightExitX = 1710.f;    // Player exit on right
inline const float LeftEntryX = 275.f;     // Entry position from left
inline const float RightEntryX = 1645.f;   // Entry position from right
inline const float BattleX = 275.f;   // Entry position of BattleScene
inline const float PlayerGroundY = 860.f;
inline const float PlayerHeight = 454.f;
inline const float PlayerStartX = 300.f;
inline const int KeyCntToOpenDoor = 3;

struct GameData {
    int score = 0;
    int level = 0;
};
