#include <iostream>
#include "gameApp.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"
#include <windows.h>

using namespace std;

player p;
screen game_screen(130, 70);
mesh m;

void start(){
    m.load_model("models/Suzane.obj");
}

void main_loop(){
    float move_speed = 0.01;
    float rotation_speed = 0.001;

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
    //apertou f
    if(GetKeyState(0x46) & 0x8000){
        cout<<"apertou f"<<endl;
        m.rotation.x += rotation_speed;
        m.rotation.y += rotation_speed;
        m.rotation.z += rotation_speed;
    }
    game_screen.gotoxy(0, 0);
    render_mesh(&game_screen, m);
    game_screen.draw_screen();
    game_screen.clear_screen();
    //p.loop();
}
