#include "uiStuff.h"
#include <iostream>
#include <windows.h>
#include "vector.h"
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <cwchar>

int maximum_screen_y(){
    int monitor_height = GetSystemMetrics(SM_CYSCREEN);
    int resolution_offset = 80;
    int minimum_font_y = 2;
    return  (monitor_height - resolution_offset) / (minimum_font_y);
}

void prepare_screen_for_rendering(int screen_width, int screen_height){
    int monitor_height = GetSystemMetrics(SM_CYSCREEN);
    int resolution_offset = 80;
    // Calculating the bigger font resolution based on screen resolution
    // Using window heigth as base because it is generally smaller
    int minimum_font_y = 2;

    int maximum_font_size = (monitor_height - resolution_offset) / (screen_height);
    maximum_font_size = (monitor_height - resolution_offset) / (screen_height + screen_height/maximum_font_size);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set console font size
    CONSOLE_FONT_INFOEX cfi;

    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = maximum_font_size;
    cfi.dwFontSize.Y = maximum_font_size;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    wcscpy_s(cfi.FaceName, L"Courier New");

    if(SetCurrentConsoleFontEx(hConsole, FALSE, &cfi)){
        std::cout<<"consegui mudar tamanho da fonte"<<std::endl;
    }else{
        std::cout<<"nao consegui mudar tamanho da fonte"<<std::endl;
    }

    // Change console buffer size
    COORD console_size;
    console_size.X = screen_width + 10;
    console_size.Y = screen_height + 10;
    if(SetConsoleScreenBufferSize(hConsole, console_size)){
        std::cout<<"consegui mudartamanho do buffers"<<std::endl;
    }else{
        std::cout<<"nao consegui mudar tamanho do buffer"<<std::endl;
    }


    COORD max_window_size = GetLargestConsoleWindowSize(hConsole);

    int window_size_x = screen_width + 9;
    int window_size_y = screen_height + 9;

    if(window_size_y > max_window_size.Y)
        window_size_y = max_window_size.Y-1;

    // Change window size
    SMALL_RECT window_size;
    window_size.Top = 0;
    window_size.Left = 0;
    window_size.Right = window_size_x;
    window_size.Bottom =  window_size_y;
    if(SetConsoleWindowInfo(hConsole, TRUE, &window_size)){
        std::cout<<"consegui mudar tamanho da janela"<<std::endl;
    }else{
        std::cout<<"nao consegui mudar tamanho da janela"<<std::endl;
    }
}

void calculate_recommended_resolutions(int start_y, int step, std::vector<vector2i> *resolutions){
    int current_screen_y = start_y;
    while(current_screen_y <= maximum_screen_y()){
        vector2i resolution;
        resolution.x = float(current_screen_y) * 1.5;
        resolution.y = current_screen_y;
        resolutions->push_back(resolution);
        current_screen_y += step;
    }
}

int get_user_input_int(std::string text, int from, int to){
    std::string input;
    int choice = -1;

    while(choice < 0 || choice > to){
        std::cout<<text;
        getline(std::cin, input);
        try{
            choice = std::stoi(input);
            if(choice < 0 || choice > to)
                throw 555;
        }catch(...){
            std::cout<<"Not a valid option!"<<std::endl;
        }
    }
    return choice;
}

vector2i ask_and_get_user_resolution(){
    std::vector<vector2i> resolutions;
    calculate_recommended_resolutions(50, 50, &resolutions);

    std::cout<<std::endl<<"Generated resolutions recommended for you screen size:"<<std::endl<<std::endl;

    for(int i = 0; i < resolutions.size(); i++){
        std::cout<<"("<<i<<") "<<resolutions[i].x<<"x"<<resolutions[i].y<<std::endl;
    }

    int choice = get_user_input_int("Choose a resolution: ", 0, int(resolutions.size()-1));

    return resolutions[choice];
}

std::string ask_and_get_user_model(){
    DIR *dr;

    char models_path[] = ".\\models";
    dr = opendir(models_path);

    if(dr == NULL){
        perror("Could not open models directory");
        return "1";
    }
    dirent *drir = readdir(dr);

    int models_quantity = 0;
    std::vector<std::string> all_models_paths;

    std::cout<<"All the models in \"/models\" folder:"<<std::endl<<std::endl;

    while(drir != NULL){
        if(strcmp(drir->d_name, ".") != 0 && strcmp(drir->d_name, "..") != 0){
            std::cout<<"("<<models_quantity<<") "<<drir->d_name<<std::endl;

            std::string model_name = drir->d_name;
            model_name.erase(model_name.length() - 3);

            all_models_paths.push_back(model_name);
            models_quantity++;
        }
        drir = readdir(dr);
    }

    int choice = get_user_input_int("Choose the model you want to load: ", 0, int(all_models_paths.size()-1));
    return all_models_paths[choice];
}
