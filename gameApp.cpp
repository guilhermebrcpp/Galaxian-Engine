#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>
#include "camera.h"
#include "inputHandler.h"

using namespace std;

player p;
screen game_screen(200, 155);
mesh modelo1;
mesh modelo2;
camera c;

void start(std::string model_name){
    //todo:
    //adicionar edge conventions seja la oq for isso
    //melhorar codigo das luzes
    //melhorar codigo de triangulo sem SIMD
    //crash quando nao carrega textura nem modelo

    //.replace(materials[i].texture_file_name.length()-3, 3, "txt");
    modelo1.load_material("mtls/" + model_name + "mtl");
    modelo1.load_model("models/" + model_name +"obj");

    modelo1.scale.set(10.1, 10.1, 10.1);
    p.start();
}

void main_loop(float delta){
    p.loop(delta);
    game_screen.gotoxy(0, 0);
    update_keys_check();

    render_mesh(&game_screen, &modelo1, p.c);
    game_screen.draw_screen();
    game_screen.clear_screen();
}
