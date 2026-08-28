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
#include "font_loader.h"

player p;
screen game_screen(400, 300);
mesh modelo1;
mesh modelo2;
camera c;
vector<std::string> numbers;
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
    std::cout<<"Welcome to the Galaxian 3D renderer."<<std::endl;
    std::cout<<"To render a custom model you should move the file to the assets/models folder."<<std::endl;
    std::cout<<"Camera controls: WASD for horizonal movement. EQ for vertical movement, IJKL for camera rotation."<<std::endl;
    std::string model_name = ask_and_get_user_model();
    vector2i resolution = ask_and_get_user_resolution();

    game_screen.set_screen_resolution(resolution.x , resolution.y);

    prepare_screen_for_rendering(game_screen.get_screen_width(), game_screen.get_screen_height());

    load_material("assets/mtls/" + model_name + ".mtl", &modelo1);
    load_model("assets/models/" + model_name + ".obj", &modelo1);

    modelo1.scale.set(10.1, 10.1, 10.1);
    p.start();

    load_font("assets/fonts/default_numbers.txt", &numbers);
}

float time_passed = 0;
int frame_quantity = 0;
int current_fps = 0;

void main_loop(float delta){
    p.loop(delta);
    game_screen.gotoxy(0, 0);
    update_keys_check();

    render_mesh(&game_screen, &modelo1, p.c);
    time_passed += delta;
    frame_quantity++;
    // If half a second has passed update the number in fps counter
    if(time_passed >= 0.96/2){
        current_fps = 1 / (time_passed / frame_quantity);
        frame_quantity = 0;
        time_passed = 0;
    }
    game_screen.draw_number(numbers, current_fps, 0, 1);
    game_screen.draw_screen();
    game_screen.clear_screen();
}
