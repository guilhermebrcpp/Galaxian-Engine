#pragma once
#include <vector>
#include "vector.h"
#include <fstream>

class mesh{
public:
    std::vector <float> vertices;
    std::vector <int> triangles;
    vector3 pos;
    vector3 rotation;

    mesh(){
        pos.set(5, 5, -70);
        rotation.set(0, 0, 0);
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

        //check if the file was oppened succefully
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file at: \""<<file_path<<"\"!."<< std::endl;
            system("pause");
            return;
        }


        std::string line;

        while (std::getline(inputFile, line)) {
            //reads vertices data
            if(starts_with("v ", line)){
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
            //reads face data
            else if(starts_with("f ", line)){
                std::string number;
                bool new_number = true;
                int vertices_count = 1;
                for(int i = 2; i < line.length(); i++){
                    if(new_number == true){
                        if(line[i] != '/' && i+1 < line.length())
                            number += line[i];
                        else{
                            if(vertices_count > 3){
                                triangles.push_back(triangles[triangles.size()-3]);
                                triangles.push_back(triangles[triangles.size()-2]);
                            }
                            triangles.push_back(std::stof(number));
                            new_number = false;
                            vertices_count++;
                        }
                    }else{
                        if(line[i] == ' '){
                            number = "";
                            new_number = true;
                            continue;
                        }
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
        for(int i = 0; i < triangles.size(); i+=3){
            std::cout<<"triangulo "<<(i+1)/3<<" valor:"<<triangles[i]<<","<<triangles[i+1]<<","<<triangles[i+2]<<std::endl;
        }
        system("pause");
        system("cls");
    }

};
