#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>
#include "camera.h"

using namespace std;

player p;
screen game_screen(280, 100);
mesh modelo1;
mesh modelo2;
camera c;

void start(){
    modelo1.load_material("mtls/Wii_Coconut_Mall.mtl");
    modelo1.load_model("models/Wii_Coconut_Mall.obj");
    //Wii_Coconut_Mall
    //plane text
    //modelo2.load_model("models/Mazda.obj");
    //modelo1.rotation.y = -1.57;
    //modelo1.rotation.z = 1.57;
    modelo1.scale.set(1, 1, 1);
    //modelo2.scale.set(0.01, 0.01, 0.01);
    p.start();


    //modelo1.load_texture("textures/casato1h.txt");

}

void main_loop(){
    //2C
    p.loop();
    game_screen.gotoxy(0, 0);
    render_mesh(&game_screen, modelo1, p.c);
    //render_mesh(&game_screen, modelo2, p.c, &tex1);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
