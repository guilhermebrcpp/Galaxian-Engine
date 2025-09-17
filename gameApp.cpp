#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>

using namespace std;

player p;
screen game_screen(330, 170);
mesh m;

void start(){
    m.load_model("models/Alice.obj");
}

void main_loop(){

    float move_speed = 0.3;
    float rotation_speed = 0.009;

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
    //ROTACAO
    //apertou j
    if(GetKeyState(0x4A) & 0x8000){
        cout<<"apertou j"<<endl;
        m.rotation.x += rotation_speed;
    }
    //apertou n
    if(GetKeyState(0x4E) & 0x8000){
        cout<<"apertou n"<<endl;
        m.rotation.x -= rotation_speed;
    }
    //apertou K
    if(GetKeyState(0x4B) & 0x8000){
        cout<<"apertou k"<<endl;
        m.rotation.y += rotation_speed;
    }
    //apertou M
    if(GetKeyState(0x4D) & 0x8000){
        cout<<"apertou m"<<endl;
        m.rotation.y -= rotation_speed;
    }
    //apertou l
    if(GetKeyState(0x4C) & 0x8000){
        cout<<"apertou l"<<endl;
        m.rotation.z += rotation_speed;
    }
    //apertou ,
    if(GetKeyState(0x2C) & 0x8000){
        cout<<"apertou ,"<<endl;
        m.rotation.z -= rotation_speed;
    }
    //2C
    game_screen.gotoxy(0, 0);
    render_mesh(&game_screen, m);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
