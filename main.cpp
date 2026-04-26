#include <iostream>
#include "gameApp.h"
#include <chrono>

int main(){
    std::cout<<"HELLO WORTLD"<<std::endl;
    start();
    auto lastTime = std::chrono::steady_clock::now();

    while(true){
        auto currentTime = std::chrono::steady_clock::now();
        main_loop();

        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        float frameTimeSeconds = deltaTime.count();
        std::cout<<"FPS:"<<frameTimeSeconds<<std::endl;

        lastTime = currentTime;
    }
    return 0;
}
