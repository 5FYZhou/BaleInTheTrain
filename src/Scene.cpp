#include "Scene.h"

Scene::Scene():grid(HEIGHT, std::vector<GridType>(WIDTH)) {
    // 初始化grid为默认值
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = GridType::DEFAULT;
        }
    }
}

GridType Scene::GetCell(int x, int y) const {
    if (x >= 0 && x < GetHeight() && y >= 0 && y < GetWidth()) {
        return grid[x][y];
    }
    return GridType::DEFAULT; // 返回默认值或抛出异常
}