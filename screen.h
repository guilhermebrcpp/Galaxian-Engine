#pragma once
#include <iostream>
#include <vector>
#include "vector.h"
#include <windows.h>

class screen{
public:
    screen(int screen_width, int screen_height){
        set_screen_resolution(screen_width, screen_height);
    }

    void set_screen_resolution(int screen_width, int screen_height){
        this->screen_width  = screen_width;
        this->screen_height = screen_height;

        screen_data = "";
        depth_data.clear();

        for(int i = 0; i<screen_height; i++){
            for(int j = 0; j<screen_width; j++){
                screen_data += ' ';
            }
            screen_data += '\n';
        }

        for(int j = 0; j<screen_height; j++){
            for(int i = 0; i<screen_width; i++){
                depth_data.push_back(999999999);
            }
        }
    }

    void gotoxy(short x, short y) {
        COORD coord = { x, y };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    bool is_on_screen(int x, int y){
        return (x > 0 && x < screen_width && y > 0 && y < screen_height);
    }

    void draw_pixel(int x, int y, char c){
        if(is_on_screen(x, y))
            screen_data[(screen_width * y + x)+y] = c;
    }

    float range(float x1, float y1, float x2, float y2, float x){
        return (y1-x1) * ((x-x2)/(y2-x2)) + x1;
    }

    void set_depth_data(int y, int x, float value){
        depth_data[screen_width * y + x] = value;
    }

    float get_depth_data(int y, int x){
        return depth_data[screen_width * y + x];
    }

    void draw_screen(){
        std::cout<<screen_data;
    }

    void clear_screen(){
        for(int i = 0; i<screen_height; i++){
            for(int j = 0; j<screen_width; j++){
                screen_data[(screen_width * i + j)+i] = ' ';
                depth_data[screen_width * i + j] = 999999999;
            }
        }
    }

    int get_screen_width(){
        return screen_width;
    }

    int get_screen_height(){
        return screen_height;
    }
private:
    int screen_width;
    int screen_height;

    std::string screen_data;
    std::vector<float> depth_data;
};
