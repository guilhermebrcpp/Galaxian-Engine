#pragma once
#include <vector>
#include "vector.h"


class mesh{
public:
    std::vector <float> vertices;
    vector3 pos;

    mesh(){
        pos.set(5, 3, 6);
    }

    //check if a string starts with "starter"
    bool starts_with(std::string starter, std::string str){
        for(int i = 0; i < starter.length(); i++){
            if(starter[i] != str[i])
                return false;
        }
        return true;
    }

    //load obj file at given path
    void load_model(std::string file_path){

        std::ifstream inputFile(file_path);

        if (!inputFile.is_open()) {
            std::cerr << "Error opening file at: \""<<file_path<<"\"!."<< std::endl;
            return;
        }


        std::string line;

        while (std::getline(inputFile, line)) {
            if(starts_with("v ")){
                std::string number;
                for(int i = 2; i < line.length(); i++){
                    if(line[i] != ' ' && i != line.length()-1)
                        number += line[i];
                    else{
                        vertices.push_back(std::stof(number));
                        number = "";
                    }
                }
            }
            std::cout << "Read: " << line << std::endl;
        }

        inputFile.close();

        std::cout<<"teste size:"<<vertices.size()<<std::endl;

        for(int i = 0; i < vertices.size(); i+=3){
            std::cout<<"posicao "<<(i+1)/3<<" valor:"<<vertices[i]<<","<<vertices[i+1]<<","<<vertices[i+2]<<std::endl;
        }

        system("pause");
        //system("cls");
    }

};
