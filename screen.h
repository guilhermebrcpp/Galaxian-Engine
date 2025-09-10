#pragma once
#include <iostream>
#include <vector>
#include "vector.h"
#include <windows.h>

class screen{
public:

    std::vector<std::string> screen_data;
    int screen_width;
    int screen_height;
    std::vector<float> teste;
    vector3 pos;

    screen(int screen_width, int screen_height){
        this->screen_width  = screen_width;
        this->screen_height = screen_height;

        std::string setup;
        for(int i = 0; i<screen_width; i++)
            setup += ' ';
        for(int i = 0; i<screen_height; i++)
            screen_data.push_back(setup);

        pos.set(7, 4, -5);
    }

    void gotoxy(short x, short y) {
        //#ifdef _WIN32
        COORD coord = { x, y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        //#else
        //    printf("%c[%d;%df", 0x1B, y, x);
        //#endif
    }

    bool is_on_screen(int x, int y){
        return (x > 0 && x < screen_width && y > 0 && y < screen_height);
    }

    void draw_pixel(int x, int y, char c){
        if(is_on_screen(x, y))
            screen_data[y][x] = c;
    }

    void draw_screen(){
        std::string screen_str;
        for(int i = 0; i<screen_height; i++){
            for(int j = 0; j<screen_width; j++){
                screen_str += screen_data[i][j];
            }
            screen_str += '\n';
        }
        std::cout<<screen_str;
    }

    void clear_screen(){
        for(int i = 0; i<screen_height; i++){
            for(int j = 0; j<screen_width; j++){
                screen_data[i][j] = ' ';
            }
        }
    }

};
