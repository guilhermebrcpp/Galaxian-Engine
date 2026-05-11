#include <iostream>
#include "gameApp.h"
#include <chrono>
int main(){
    std::cout<<"HELLO WORTLD"<<std::endl;
    start();
    auto lastTime = std::chrono::steady_clock::now();
    float frameTimeSeconds = 1;

    while(true){
        auto currentTime = std::chrono::steady_clock::now();
        std::cout<<"FPS:"<<frameTimeSeconds<<std::endl;
        main_loop(frameTimeSeconds);


        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        frameTimeSeconds = deltaTime.count();



        lastTime = currentTime;
    }
    return 0;
}
