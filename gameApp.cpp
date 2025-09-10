#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"

using namespace std;

player p;
screen game_screen(50, 50);
mesh m;

void start(){
    m.load_model("models/cube.obj");
}

void main_loop(){
    float move_speed = 0.01;
    //game_screen.draw_pixel(9, 9, '#');
    if(GetKeyState(0x57) & 0x8000){
            cout<<"apertou w"<<endl;
            m.pos.z -= move_speed;
        }

        //apertou a
        if(GetKeyState(0x41) & 0x8000){
            cout<<"apertou a"<<endl;
            m.pos.x -= move_speed;
        }
        //apertou d
        if(GetKeyState(0x44) & 0x8000){
            cout<<"apertou d"<<endl;
            m.pos.x += move_speed;
        }
        // apertou s
        if(GetKeyState(0x53) & 0x8000){
            cout<<"apertou s"<<endl;
            m.pos.z += move_speed;
        }
        //apertou q
        if(GetKeyState(0x51) & 0x8000){
            cout<<"apertou q"<<endl;
            m.pos.y += move_speed;
        }
        //apertou e
        if(GetKeyState(0x45) & 0x8000){
            cout<<"apertou e"<<endl;
            m.pos.y -= move_speed;
        }
    game_screen.gotoxy(0, 0);
    //draw_points(&game_screen, m.vertices, m.pos);
    //game_screen.draw_pixel(10, 10, '#');
    render_mesh(&game_screen, m);
    game_screen.draw_screen();
    //system("pause");
    game_screen.clear_screen();
    //p.loop();
}
