#pragma once

class texture{
public:
    std::string data;

    int get_width(){ return width; }
    int get_height(){ return height; }
    void set_width(int value){ width = value; }
    void set_height(int value){ height = value; }
private:
    int width;
    int height;

};
