# Snake

一个基于 Windows 控制台的贪吃蛇游戏，使用 C++17 编写。

## 项目简介

该项目实现了一个简单的贪吃蛇游戏：

- 控制台渲染游戏画面
- `WASD` 控制移动
- `Space` 暂停
- `Enter` 切换显示模式
- `Esc` 退出游戏
- 支持游戏数据保存/加载

## 主要模块

- `src/Game.cpp` / `src/Game.h`
  - 游戏主循环、逻辑控制、输入处理、绘制
- `src/Snake.cpp` / `src/Snake.h`
  - 蛇的身体管理、移动、碰撞检测
- `src/Food.cpp` / `src/Food.h`
  - 食物生成与位置管理
- `src/Scene.cpp` / `src/Scene.h`
  - 游戏场景构建、网格绘制
- `src/Renderer.cpp` / `src/Renderer.h`
  - 控制台绘制与光标定位
- `src/Input.cpp` / `src/Input.h`
  - 键盘输入读取与命令解析
- `src/SaveSystem.h`
  - 游戏进度保存与加载
- `src/Constants.h`
  - 全局常量、枚举类型、通用数据结构

## 构建要求

- Windows 平台
- C++17 支持
- CMake 3.28+
- 可选择 MinGW/GCC 或其他支持 C++17 的编译器

## 构建与运行

```powershell
cd d:\VSCodeProjects\GameInCPlusPlus\Snake
mkdir build
cd build
cmake ..
cmake --build .
```

生成可执行文件后：

```powershell
cd build\bin
./main.exe
```

> `CMakeLists.txt` 会将 `data` 目录复制到 `build/bin/data`，用于保存游戏数据文件 `GameData.txt`。

## 控制说明

- `W`：向上移动
- `S`：向下移动
- `A`：向左移动
- `D`：向右移动
- `Space`：切换暂停状态
- `Enter`：切换渲染模式（全屏 / 半屏）
- `Esc`：退出游戏

## 保存与继续

- 项目会将游戏数据保存到 `data/GameData.txt`
- 退出后再次运行会提示是否继续之前存档

## 目录结构

```text
Snake/
├── CMakeLists.txt
├── data/
│   └── GameData.txt
├── src/
│   ├── Constants.h
│   ├── Food.cpp
│   ├── Food.h
│   ├── Game.cpp
│   ├── Game.h
│   ├── Input.cpp
│   ├── Input.h
│   ├── main.cpp
│   ├── Renderer.cpp
│   ├── Renderer.h
│   ├── SaveSystem.h
│   ├── Scene.cpp
│   ├── Scene.h
│   ├── Snake.cpp
│   └── Snake.h
└── README.md
```

## 备注

- 当前仓库链接为 `GameInCPlusPlus/Snake`
- 如果需要，我可以继续补充：作者信息、许可证、GitHub 仓库 URL、截图或示例运行效果。

以上内容由Github Copilot生成

作者：FYZhou