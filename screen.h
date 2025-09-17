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
    std::vector<std::vector<float>> depth_data;
    vector3 pos;

    screen(int screen_width, int screen_height){
        this->screen_width  = screen_width;
        this->screen_height = screen_height;

        std::string setup;
        for(int i = 0; i<screen_width; i++)
            setup += ' ';
        for(int i = 0; i<screen_height; i++)
            screen_data.push_back(setup);

        std::vector<float> temp;
        for(int i = 0; i<screen_width; i++)
            temp.push_back(999999999);
        for(int j = 0; j<screen_height; j++)
            depth_data.push_back(temp);

        pos.set(7, 4, -5);
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
            screen_data[y][x] = c;
    }

    float range(float x1, float y1, float x2, float y2, float x){
        return (y1-x1) * ((x-x2)/(y2-x2)) + x1;
    }

    void draw_screen(){
        std::string colors = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:.\"^'. ";
        //" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"

        std::string screen_str;
        /*
        for(int i = 0; i<screen_height; i++){
            for(int j = 0; j<screen_width; j++){
                screen_str += colors[std::min(int(colors.length()-1), std::max(0, int(range(0, colors.length()-1, -1, 0, depth_data[i][j]))))];
            }
            screen_str += '\n';
        }*/

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
                depth_data[i][j] = 999999999;
            }
        }
    }

};
