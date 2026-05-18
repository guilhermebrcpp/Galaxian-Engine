#include <iostream>
#include <windows.h>

#include "uiStuff.h"
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include "camera.h"
#include "inputHandler.h"
#include "objParser.h"

using namespace std;

player p;
screen game_screen(400, 300);
mesh modelo1;
mesh modelo2;
camera c;

void start(){
    //todo:
    //adicionar edge conventions seja la oq for isso
    //melhorar codigo das luzes
    //melhorar codigo de triangulo sem SIMD
    //crash quando nao carrega textura nem modelo
    //- tratar input do usuario
    //botar resolucao customizada
    //configuracoes avançadas
    //fazer ele verificar se o arquivo passado realmente é um .obj

    std::string model_name = ask_and_get_user_model();
    vector2i resolution = ask_and_get_user_resolution();

    game_screen.set_screen_resolution(resolution.x , resolution.y);

    prepare_screen_for_rendering(game_screen.get_screen_width(), game_screen.get_screen_height());

    load_material("mtls/" + model_name + "mtl", &modelo1);
    load_model("models/" + model_name +"obj", &modelo1);

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
