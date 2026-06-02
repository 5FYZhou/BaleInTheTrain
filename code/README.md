# Bale In The Train - SFML 最小游戏框架

这是一个适合大学课程设计的 SFML 3 + CMake + MinGW + VSCode 项目框架。

## 目录结构

```text
bale in the train/
├─ assets/
│  └─ background.png
└─ code/
   ├─ CMakeLists.txt
   ├─ src/
   │  └─ main.cpp
   ├─ include/
   │  └─ README.txt
   └─ build/
      └─ README.txt
```

## 编译命令

在 `code` 文件夹中打开终端，然后执行：

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

如果 CMake 找不到 SFML，需要手动指定 SFML 3 路径，例如：

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DSFML_DIR="D:/SFML/lib/cmake/SFML"
cmake --build build
```

## 运行说明

程序会读取：

```text
../assets/background.png
```

也就是 `code` 文件夹上一级目录中的 `assets/background.png`。

如果图片加载失败，程序会在控制台输出错误信息。
