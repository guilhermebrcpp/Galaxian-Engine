#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>
#include "camera.h"

using namespace std;

player p;
screen game_screen(480, 390);
mesh modelo1;
camera c;
texture tex1;

void start(){
    modelo1.load_model("models/Sheik.obj");
    modelo1.scale.set(1, 1, 1);
    p.start();


    modelo1.load_texture("textures/sheik.txt");

}

void main_loop(){
    //2C
    p.loop();
    game_screen.gotoxy(0, 0);
    render_mesh(&game_screen, modelo1, p.c, &tex1);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
