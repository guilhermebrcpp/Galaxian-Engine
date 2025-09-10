#pragma once
#include <iostream>
#include <windows.h>
#include "vector.h"

using namespace std;

class player{
public:
    vector3 pos;
    float move_speed = 1;

    void input(){
        //apertou w
        if(GetKeyState(0x57) & 0x8000){
            cout<<"apertou w"<<endl;
            pos.z -= move_speed;
        }

        //apertou a
        if(GetKeyState(0x41) & 0x8000){
            cout<<"apertou a"<<endl;
            pos.x -= move_speed;
        }
        //apertou d
        if(GetKeyState(0x44) & 0x8000){
            cout<<"apertou d"<<endl;
            pos.x += move_speed;
        }
        // apertou s
        if(GetKeyState(0x53) & 0x8000){
            cout<<"apertou s"<<endl;
            pos.z += move_speed;
        }
    }

    void render(){

    }

    void loop(){
        input();
        render();
    }

};
