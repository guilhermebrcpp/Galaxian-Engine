#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>
#include "camera.h"
using namespace std;

player p;
screen game_screen(330, 170);
mesh m;
camera c;
void start(){
    m.load_model("models/Alice.obj");
}

void main_loop(){
    //2C
    p.loop();
    game_screen.gotoxy(0, 0);
    render_mesh(&game_screen, m, p.c);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
