#pragma once
#include <vector>
#include "vector.h"
#include <fstream>
#include "texture.h"

class mesh{
public:
    std::vector <float> vertices;
    std::vector <int> triangles;
    std::vector <vector2> vertex_texture;
    std::vector <int> vertex_texture_indices;
    vector3 pos;
    vector3 rotation;
    vector3 scale;
    texture mesh_texture;

    mesh(){
        pos.set(0, 0, 0);//1.5708*2
        rotation.set(0, 0, 0);
        scale.set(1, 1, 1);
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
                //variable to keep track of which number in the triplet the loop currently is in
                int which_number = 0;
                //f 1/1/1 2/2/1 4/3/1 3/4/1
                int vertex_count = 0;
                std::string number = "";
                for(int i = 2; i < line.length(); i++){

                    if(line[i] != '/' && line[i] != ' '){
                        number += line[i];
                    }else{
                        if(line[i] == ' '){
                            which_number = 0;
                            number = "";
                            continue;
                        }
                        switch(which_number){
                        //get vertex position
                        case 0:
                            if(vertex_count >= 3){
                                triangles.push_back(triangles[triangles.size()-3]);
                                triangles.push_back(triangles[triangles.size()-2]);
                            }
                            triangles.push_back(std::stoi(number));
                            vertex_count++;
                            break;
                        //get vertex texture position of a vertex
                        case 1:
                            if(vertex_count > 3){
                                vertex_texture_indices.push_back(vertex_texture_indices[vertex_texture_indices.size()-3]);
                                vertex_texture_indices.push_back(vertex_texture_indices[vertex_texture_indices.size()-2]);
                            }
                            vertex_texture_indices.push_back(std::stoi(number));
                            break;
                        //get normal position
                        case 2:
                            break;
                        }
                        number = "";
                        which_number++;
                    }
                }
            }
            //reads vertex texture data
            else if(starts_with("vt ", line)){
                std::string number;
                vector2 new_vec2;
                bool first_num = false;
                for(int i = 3; i < line.length(); i++){
                    if(line[i] != ' ' && i != line.length()-1)
                        number += line[i];
                    else{
                        if(first_num == false){
                            new_vec2.x = std::stof(number);
                            first_num = true;
                        }else
                            new_vec2.y = std::stof(number);
                        number = "";
                    }
                }
                vertex_texture.push_back(new_vec2);
            }
            //std::cout << "Read: " << line << std::endl;
        }

        inputFile.close();

        std::cout<<"texture vertex quantity:"<<vertex_texture.size()<<std::endl;
        std::cout<<"vertex quantity:"<<vertices.size()<<std::endl;
        std::cout<<"triangle quantity:"<<triangles.size()<<std::endl;

        /*
        for(int i = 0; i < vertices.size(); i+=3){
            std::cout<<"posicao "<<(i+1)/3<<" valor:"<<vertices[i]<<","<<vertices[i+1]<<","<<vertices[i+2]<<std::endl;
        }

        for(int i = 0; i < vertex_texture.size(); i+=1){
            std::cout<<"vertex: "<<i<<" valor x:"<<vertex_texture[i].x<<"  valor y:"<<vertex_texture[i].y<<std::endl;
        }
        for(int i = 0; i < triangles.size(); i+=3){
            std::cout<<"triangulo "<<(i+1)/3<<" valor:"<<triangles[i]<<","<<triangles[i+1]<<","<<triangles[i+2]<<std::endl;
        }*/
        system("pause");
        system("cls");
    }
    void set_texture(texture tex){
        mesh_texture = tex;
        has_texture = true;
    }
    bool have_texture(){
        return has_texture;
    }
    void load_texture(std::string file_path){

        std::ifstream inputFile(file_path);

        //check if the file was oppened succefully
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file at: \""<<file_path<<"\"!."<< std::endl;
            system("pause");
            return;
        }


        std::string line;

        int tex_y = 0;
        while (std::getline(inputFile, line)) {
            mesh_texture.data.push_back(line);//[tex_y][tex_x] = line[tex_x];
            //tex_y++;
        }
        inputFile.close();

        std::cout<<"width:"<<mesh_texture.data[0].length()<<std::endl;
        std::cout<<"height:"<<mesh_texture.data.size()<<std::endl;
        system("pause");
        /*
        for(int i = 0; i<mesh_texture.get_height(); i++){
            for(int j = 0; j<mesh_texture.get_width(); j++){
                std::cout<<mesh_texture.data[i][j];
            }
            std::cout<<std::endl;
        }*/
        has_texture = true;
    }
private:
    bool has_texture = false;
};
