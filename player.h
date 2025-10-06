#pragma once
#include <iostream>
#include <windows.h>
#include "vector.h"
#include <math.h>
#include "camera.h"
#include "mesh.h"
using namespace std;

class player{
public:
    vector3 pos;
    vector3 rotation;
    camera c;
    float move_speed = 0.09;
    float rotation_speed = 0.009;
    mesh aviao;

    void start(){
        pos.set(0, 0, 10);
        rotation.set(0, 0, 0);
    }

    void input(){
        if(GetKeyState(0x57) & 0x8000){
        cout<<"apertou w"<<endl;
        pos.z += cos(c.rotation.y) * move_speed;
        pos.x += sin(c.rotation.y) * move_speed;
        pos.y += -sin(c.rotation.x) * move_speed;
    }
    // apertou s
    if(GetKeyState(0x53) & 0x8000){
        cout<<"apertou s"<<endl;
        pos.z -= cos(c.rotation.y) * move_speed;
        pos.x -= sin(c.rotation.y) * move_speed;
        pos.y -= -sin(c.rotation.x) * move_speed;
    }
        //apertou a
    if(GetKeyState(0x41) & 0x8000){
        cout<<"apertou a"<<endl;
        pos.x -= cos(c.rotation.y) * move_speed;
        pos.z -= -sin(c.rotation.y) * move_speed;
    }
    //apertou d
    if(GetKeyState(0x44) & 0x8000){
        cout<<"apertou d"<<endl;
        pos.x += cos(c.rotation.y) * move_speed;
        pos.z += -sin(c.rotation.y) * move_speed;
    }
    //apertou q
    if(GetKeyState(0x51) & 0x8000){
        cout<<"apertou q"<<endl;
        pos.y -= move_speed;
    }
    //apertou e
    if(GetKeyState(0x45) & 0x8000){
        cout<<"apertou e"<<endl;
        pos.y += move_speed;
    }
    //ROTACAO
    //apertou k
    if(GetKeyState(0x4B) & 0x8000){
        cout<<"apertou k"<<endl;
        if(rotation.x < 1.5708)
            rotation.x += rotation_speed;
        else
            rotation.x = 1.5708;
    }
    //apertou i
    if(GetKeyState(0x49) & 0x8000){
        cout<<"apertou i"<<endl;
        if(rotation.x > -1.5708)
            rotation.x -= rotation_speed;
        else
            rotation.x = -1.5708;
    }
    //apertou j
    if(GetKeyState(0x4A) & 0x8000){
        cout<<"apertou j"<<endl;
        rotation.y -= rotation_speed;
    }
    //apertou l
    if(GetKeyState(0x4C) & 0x8000){
        cout<<"apertou l"<<endl;
        rotation.y += rotation_speed;
    }
    }

    void render(){

    }

    void loop(){
        aviao.pos.set(pos);
        //offset
        aviao.pos.add(0, -0.5, 1);
        //aviao.rotation = rotation;
        c.pos = pos;
        c.rotation = rotation;
        input();
        render();
    }

};
