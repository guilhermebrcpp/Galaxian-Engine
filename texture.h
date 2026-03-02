#pragma once

class texture{
public:
    std::vector<std::string> data;

    int get_width(){ return data[0].size(); }
    int get_height(){ return data.size(); }
    int set_width(int value){ width = value; }
    int set_height(int value){ height = value; }
private:
    int width;
    int height;

};
