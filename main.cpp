#include <iostream>
#include "gameApp.h"

int main(){
    std::cout<<"HELLO WORTLD"<<std::endl;
    start();
    while(true){
        main_loop();
    }
    return 0;
}
