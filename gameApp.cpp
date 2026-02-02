#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>
#include "camera.h"
using namespace std;

player p;
screen game_screen(180, 100);
mesh predio1;
mesh predio2;
camera c;

void start(){
    predio1.load_model("models/rio.obj");
    predio1.scale.set(10, 10, 10);
    predio2.pos.set(2, 0, 0);
    p.aviao.scale.set(0.001, 0.001, 0.001);
    p.aviao.pos.set(2, 0, -3);
    p.start();
}

void main_loop(){
    //2C
    p.loop();
    game_screen.gotoxy(0, 0);
    render_mesh(&game_screen, predio1, p.c);
    //render_mesh(&game_screen, predio2, p.c);
    //render_mesh(&game_screen, p.aviao, p.c);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
