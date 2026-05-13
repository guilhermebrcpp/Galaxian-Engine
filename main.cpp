#include <iostream>
#include "gameApp.h"
#include <chrono>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>
#include <cwchar>

void prepare_screen_for_rendering(){
    //set font size
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFOEX cfi;

    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 9;
    cfi.dwFontSize.Y = 18;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    wcscpy_s(cfi.FaceName, L"Consolas");

    if(SetCurrentConsoleFontEx(hConsole, FALSE, &cfi)){
        std::cout<<"consegui mudar tamanho da fonte"<<std::endl;
    }else{
        std::cout<<"nao consegui mudar tamanho da fonte"<<std::endl;
    }

    // Change window size
    SMALL_RECT window_size;
    window_size.Top = 0;
    window_size.Left = 0;
    window_size.Right = 750;
    window_size.Bottom = 480;
    if(SetConsoleWindowInfo(hConsole, TRUE, &window_size)){
        std::cout<<"consegui mudar tamanho da janela"<<std::endl;
    }else{
        std::cout<<"nao consegui mudar tamanho da janela"<<std::endl;
    }

    // Change console buffer size
    COORD console_size;
    console_size.X = 350;
    console_size.Y = 250;
    if(SetConsoleScreenBufferSize(hConsole, console_size)){
        std::cout<<"consegui mudartamanho do buffers"<<std::endl;
    }else{
        std::cout<<"nao consegui mudar tamanho do buffer"<<std::endl;
    }
}

int main(){
    DIR *dr;

    char models_path[] = ".\\models";
    dr = opendir(models_path);

    if(dr == NULL){
        perror("Could not open directory");
        return 1;
    }
    dirent *drir = readdir(dr);

    int models_quantity = 0;
    std::vector<std::string> all_models_paths;

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

    int monitor_width = GetSystemMetrics(SM_CXSCREEN);
    int monitor_height = GetSystemMetrics(SM_CYSCREEN);

    std::cout<<"X:"<<monitor_width<<" Y:"<<monitor_height<<std::endl;

    int choice;
    cin>>choice;

    // Initialize stuff
    start(all_models_paths[choice]);

    prepare_screen_for_rendering();

    system("pause");

    auto lastTime = std::chrono::steady_clock::now();
    float frameTimeSeconds = 1;



    while(true){
        auto currentTime = std::chrono::steady_clock::now();
        std::cout<<"FPS:"<<frameTimeSeconds<<std::endl;

        main_loop(frameTimeSeconds);

        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        frameTimeSeconds = deltaTime.count();
        lastTime = currentTime;
    }
    return 0;
}
