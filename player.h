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
    float move_speed = 500;
    float rotation_speed = 1;

    void start(){
        pos.set(-1.5, 0, 10);
        rotation.set(0, 3.14, 0);
    }

    void input(float delta){
        if(GetKeyState(0x57) & 0x8000){
        cout<<"apertou w"<<endl;
        pos.z += cos(c.rotation.y) * move_speed * delta;
        pos.x += sin(c.rotation.y) * move_speed * delta;
        pos.y += -sin(c.rotation.x) * move_speed * delta;
    }
    // apertou s
    if(GetKeyState(0x53) & 0x8000){
        cout<<"apertou s"<<endl;
        pos.z -= cos(c.rotation.y) * move_speed * delta;
        pos.x -= sin(c.rotation.y) * move_speed * delta;
        pos.y -= -sin(c.rotation.x) * move_speed * delta;
    }
        //apertou a
    if(GetKeyState(0x41) & 0x8000){
        cout<<"apertou a"<<endl;
        pos.x -= cos(c.rotation.y) * move_speed * delta;
        pos.z -= -sin(c.rotation.y) * move_speed * delta;
    }
    //apertou d
    if(GetKeyState(0x44) & 0x8000){
        cout<<"apertou d"<<endl;
        pos.x += cos(c.rotation.y) * move_speed * delta;
        pos.z += -sin(c.rotation.y) * move_speed * delta;
    }
    //apertou q
    if(GetKeyState(0x51) & 0x8000){
        cout<<"apertou q"<<endl;
        pos.y -= move_speed * delta;
    }
    //apertou e
    if(GetKeyState(0x45) & 0x8000){
        cout<<"apertou e"<<endl;
        pos.y += move_speed * delta;
    }
    //ROTACAO
    //apertou k
    if(GetKeyState(0x4B) & 0x8000){
        cout<<"apertou k"<<endl;
        if(rotation.x < 1.5708)
            rotation.x += rotation_speed * delta;
        else
            rotation.x = 1.5708;
    }
    //apertou i
    if(GetKeyState(0x49) & 0x8000){
        cout<<"apertou i"<<endl;
        if(rotation.x > -1.5708)
            rotation.x -= rotation_speed * delta;
        else
            rotation.x = -1.5708;
    }
    //apertou j
    if(GetKeyState(0x4A) & 0x8000){
        cout<<"apertou j"<<endl;
        rotation.y -= rotation_speed * delta;
    }
    //apertou l
    if(GetKeyState(0x4C) & 0x8000){
        cout<<"apertou l"<<endl;
        rotation.y += rotation_speed * delta;
    }
    }

    void render(){

    }

    void loop(float delta){
        //offset
        c.pos = pos;
        c.rotation = rotation;
        input(delta);
        render();
    }

};
