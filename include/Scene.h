#pragma once

#include "Constants.h"
#include <vector>

class Scene {
private:
    std::vector<std::vector<GridType>> grid;
    std::vector<std::vector<bool>> revealed; // 记录每个格子是否被揭示
    std::vector<std::vector<bool>> flagged;  // 标记旗帜
    std::vector<std::vector<bool>> questioned; // 标记问号
    int width, height, mineNum;
    bool firstClick; // 首次点击避免踩雷
    bool gameOver;
    bool win;
    int cellsLeft; // 未揭示非雷格数量
    int flagCount;
    int questionCount;
    int openCount;
public:
    Scene();
    void Reset(int w, int h, int m);
    void PlaceMines(int safeX, int safeY);
    void CalculateNumbers();
    bool Reveal(int x, int y); // 揭示格子，返回是否踩雷
    void RevealAll();
    void ToggleFlag(int x, int y);
    void ToggleQuestion(int x, int y);
    void QuickOpen(int x, int y); // 双击快速展开
    bool IsMine(int x, int y) const;
    bool IsFlagged(int x, int y) const;
    bool IsQuestioned(int x, int y) const;
    bool IsRevealed(int x, int y) const;
    GridType GetCell(int x, int y) const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetMineNum() const { return mineNum; }
    int GetFlagCount() const { return flagCount; }
    int GetQuestionCount() const { return questionCount; }
    int GetOpenCount() const { return openCount; }
    bool IsGameOver() const { return gameOver; }
    bool IsWin() const { return win; }
    void CheckWin();
    const std::vector<std::vector<GridType>>& GetScene() const { return grid; }
};