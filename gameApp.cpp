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
screen game_screen(500, 250);
mesh modelo1;
mesh modelo2;
camera c;

void start(){
    //todo:fazer o programa nao crashar se nao tiver um mtl
    //adicionar suporte a matrizes 4x4
    //adicionar edge conventions seja la oq for isso
    //ver pq q o parser ta uma merda (// //)


    modelo1.load_material("mtls/model.mtl");
    modelo1.load_model("models/model.obj");
    //model
    //Wii_Coconut_Mall
    //plane text
    //modelo2.load_model("models/Mazda.obj");
    //modelo1.rotation.y = -1.57;
    //modelo1.rotation.z = 1.57*2;
    modelo1.scale.set(110.1, 110.1, 110.1);
    //modelo2.scale.set(0.01, 0.01, 0.01);
    p.start();
}

void main_loop(float delta){
    //2C
    p.loop(delta);
    game_screen.gotoxy(0, 0);
    update_keys_check();

    if(is_key_pressed('P')){
        //system("pause");
        p.c.cam_near += 1.1;
        printf("mudei o bagulho e agr ta: %f                     \n", p.c.cam_near);
    }
    //system("cls");
    render_mesh(&game_screen, &modelo1, p.c);
    //render_mesh(&game_screen, modelo2, p.c, &tex1);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
