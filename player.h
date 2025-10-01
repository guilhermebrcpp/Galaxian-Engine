#pragma once
#include <iostream>
#include <windows.h>
#include "vector.h"
#include "camera.h"
using namespace std;

class player{
public:
    vector3 pos;
    camera c;
    float move_speed = 0.3;
    float rotation_speed = 0.009;

    void input(){
        if(GetKeyState(0x57) & 0x8000){
        cout<<"apertou w"<<endl;
        c.pos.z += move_speed;
    }
    // apertou s
    if(GetKeyState(0x53) & 0x8000){
        cout<<"apertou s"<<endl;
        c.pos.z -= move_speed;
    }
        //apertou a
    if(GetKeyState(0x41) & 0x8000){
        cout<<"apertou a"<<endl;
        c.pos.x -= move_speed;
    }
    //apertou d
    if(GetKeyState(0x44) & 0x8000){
        cout<<"apertou d"<<endl;
        c.pos.x += move_speed;
    }
    //apertou q
    if(GetKeyState(0x51) & 0x8000){
        cout<<"apertou q"<<endl;
        c.pos.y += move_speed;
    }
    //apertou e
    if(GetKeyState(0x45) & 0x8000){
        cout<<"apertou e"<<endl;
        c.pos.y -= move_speed;
    }
    //ROTACAO
    //apertou i
    if(GetKeyState(0x49) & 0x8000){
        cout<<"apertou i"<<endl;
        c.rotation.x += rotation_speed;
    }
    //apertou k
    if(GetKeyState(0x4B) & 0x8000){
        cout<<"apertou k"<<endl;
        c.rotation.x -= rotation_speed;
    }
    //apertou j
    if(GetKeyState(0x4A) & 0x8000){
        cout<<"apertou j"<<endl;
        c.rotation.y += rotation_speed;
    }
    //apertou l
    if(GetKeyState(0x4C) & 0x8000){
        cout<<"apertou l"<<endl;
        c.rotation.y -= rotation_speed;
    }
    }

    void render(){

    }

    void loop(){
        input();
        render();
    }

};
