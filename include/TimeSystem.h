#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <chrono>
using namespace std::chrono;

class TimeSystem {
    public:
        TimeSystem() : lastTime(steady_clock::now()) {}
        float GetDeltaTime() {
            auto currentTime = steady_clock::now();
            std::chrono::duration<float> delta = currentTime - lastTime;
            lastTime = currentTime;
            return delta.count();
        }
    private:
        steady_clock::time_point lastTime;
};