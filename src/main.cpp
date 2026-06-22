#include "Game.h"
#include <windows.h>
int main() { 
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    Game game;
    game.Run();
    return 0;
}