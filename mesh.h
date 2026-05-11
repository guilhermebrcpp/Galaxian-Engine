#pragma once
#include <vector>
#include "vector.h"
#include <fstream>
#include "texture.h"
#include "material.h"

class mesh{
public:
    //need them separated for later optimizations using SIMD (this makes me very sad)
    std::vector <float> vertices_x;
    std::vector <float> vertices_y;
    std::vector <float> vertices_z;
    std::vector <std::vector<int>> triangles;
    std::vector <std::string> materials_used_in_order;
    std::vector <vector2> vertex_texture;
    std::vector <std::vector<int>> vertex_texture_indices;
    vector3 pos;
    vector3 rotation;
    vector3 scale;
    std::vector<texture> mesh_texture;
    std::vector<material> materials;

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
        std::vector<int> sub_mesh;
        std::vector<int> sub_texture_mesh;
        while (std::getline(inputFile, line)) {
            //reads vertices data
            if(starts_with("v ", line)){
                //std::cout<<"estou pegando v:"<<line<<std::endl;
                std::string number;
                //which current number we are in the line, the order is: x, y, z
                int which_num = 0;
                for(int i = 2; i <= line.length(); i++){
                    if(line[i] != ' ' && i != line.length())
                        number += line[i];
                    else{

                        //std::cout<<"numero q estou pegando:"<<number<<std::endl;
                        //system("pause");
                        switch(which_num){
                            case 0:
                                vertices_x.push_back(std::stof(number));
                                break;
                            case 1:
                                vertices_y.push_back(std::stof(number));
                                break;
                            case 2:
                                vertices_z.push_back(std::stof(number));
                                break;
                        }

                        which_num++;
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
                for(int i = 2; i <= line.length(); i++){

                    if(line[i] != '/' && line[i] != ' ' && i != line.length()){
                        number += line[i];
                    }else{

                        switch(which_number){
                        //get vertex position
                        case 0:
                            if(vertex_count >= 3){
                                sub_mesh.push_back(sub_mesh[sub_mesh.size()-3]);
                                sub_mesh.push_back(sub_mesh[sub_mesh.size()-2]);
                            }
                            sub_mesh.push_back(std::stoi(number));
                            vertex_count++;
                            break;
                        //get vertex texture position of a vertex
                        case 1:
                            if(vertex_count > 3){
                                sub_texture_mesh.push_back(sub_texture_mesh[sub_texture_mesh.size()-3]);
                                sub_texture_mesh.push_back(sub_texture_mesh[sub_texture_mesh.size()-2]);
                            }
                            if(number.length() != 0){
                                sub_texture_mesh.push_back(std::stoi(number));
                                //std::cout<<"adicionei:"<<number<<std::endl;
                                //system("pause");
                            }
                            break;
                        //get normal position
                        case 2:
                            break;
                        }
                        if(line[i] == ' '){
                            which_number = 0;
                            number = "";
                            continue;
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
                for(int i = 3; i <= line.length(); i++){
                    if(line[i] != ' ' && i != line.length())
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
            else if(starts_with("usemtl ", line)){
                if(sub_mesh.size() != 0){
                    triangles.push_back(sub_mesh);
                    sub_mesh.clear();
                }
                if(sub_texture_mesh.size() != 0)
                    vertex_texture_indices.push_back(sub_texture_mesh);

                sub_texture_mesh.clear();

                std::string name;
                for(int i = 7; i < line.length(); i++){
                    name += line[i];
                }
                materials_used_in_order.push_back(name);
            }
            //std::cout << "Read: " << line << std::endl;
        }
        vertex_texture_indices.push_back(sub_texture_mesh);
        triangles.push_back(sub_mesh);
        inputFile.close();

        int triangle_quantity = 0;
        for(int i = 0; i < triangles.size(); i++){
            triangle_quantity += triangles[i].size()/3;
        }
        std::cout<<"texture vertex quantity:"<<vertex_texture.size()<<std::endl;
        std::cout<<"vertex quantity:"<<vertices_x.size() + vertices_y.size() + vertices_z.size()<<std::endl;
        std::cout<<"triangle submesh quantity:"<<triangles.size()<<std::endl;
        std::cout<<"triangle quantity:"<<triangle_quantity<<std::endl;
        system("pause");

        for(int i = 0; i < triangles.size(); i++){
        }


        //for(int i = 0; i < vertices.size(); i+=3){
            //std::cout<<"posicao "<<(i+1)/3<<" valor:"<<vertices[i]<<","<<vertices[i+1]<<","<<vertices[i+2]<<std::endl;
        //}

        //for(int i = 0; i < vertex_texture.size(); i+=1){
        //    std::cout<<"vertex: "<<i<<" valor x:"<<vertex_texture[i].x<<"  valor y:"<<vertex_texture[i].y<<std::endl;
        //}


        /*for(int i = 0; i < triangles.size(); i++){
            std::cout<<"submesh "<<i<<":\n";
            for(int j = 70329*50; j < triangles[i].size(); j+=3){
                std::cout<<"triangulo "<<(j+1)/3<<" valor:"<<triangles[i][j+0]<<","<<triangles[i][j+1]<<","<<triangles[i][j+2]<<std::endl;
                printf("vertice1: %f, vertice2: %f, vertice3: %f\n", vertices[triangles[i][j+0]], vertices[triangles[i][j+1]], vertices[triangles[i][j+2]]);
            }
            //system("pause");
        }*/
        //system("pause");
        //system("cls");

    }

    void set_texture(texture tex){
        mesh_texture.push_back(tex);
        has_texture = true;
    }

    bool have_texture(){
        return has_texture;
    }

    void load_texture(std::string file_path, texture *tex){

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

            //std::cout<<"blablabla: "<<line<<std::endl;
            tex->data += line;
            tex->set_width(line.length());

            //[tex_y][tex_x] = line[tex_x];
            //tex_y++;
        }

        inputFile.close();


        tex->set_height(tex->data.length()/tex->get_width());

        std::cout<<"width:"<<tex->get_width()<<std::endl;
        std::cout<<"height:"<<tex->get_height()<<std::endl;
        system("pause");
        has_texture = true;
    }

    void load_material(std::string file_path){
        std::ifstream inputFile(file_path);

        //check if the file was oppened succefully
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file at: \""<<file_path<<"\"!."<< std::endl;
            system("pause");
            return;
        }

        std::string line;
        material new_material;
        while (std::getline(inputFile, line)) {
            //read name of new material
            if(starts_with("newmtl ", line)){
                if(new_material.name.length() != 0){
                    materials.push_back(new_material);
                    new_material.name ="";
                    new_material.texture_file_name ="";
                }
                for(int i = 7; i < line.length(); i++)
                    new_material.name += line[i];
            }
            //read texture file path of new material
            else if(starts_with("map_Kd ", line)){
                for(int i = 7; i < line.length(); i++)
                    new_material.texture_file_name += line[i];
            }
        }
        materials.push_back(new_material);
        inputFile.close();

        std::cout<<"materials quantity:"<<materials.size()<<std::endl;
        for(int i = 0; i < materials.size(); i++){
            std::cout<<"nome do material:"<<materials[i].name<<std::endl;
            std::cout<<"nome da textura:"<<materials[i].texture_file_name<<std::endl;
        }

        //load all textures for the submaterials
        for(int i = 0; i < materials.size(); i++){
            if(materials[i].texture_file_name.length() == 0){
                materials[i].albedo_texture.data += "....";
                materials[i].albedo_texture.data += "....";
                materials[i].albedo_texture.data += "....";
                materials[i].albedo_texture.data += "....";
                continue;
            }

            materials[i].texture_file_name.replace(materials[i].texture_file_name.length()-3, 3, "txt");
            std::cout<<"nome blabla:"<<materials[i].texture_file_name<<std::endl;
            load_texture("textures\\" + materials[i].texture_file_name, &materials[i].albedo_texture);
        }
    }

    int get_current_material(int sub_mesh_index){
        for(int i = 0; i < materials.size(); i++){
            if(materials_used_in_order[sub_mesh_index] == materials[i].name){
                //std::cout<<"estou usando o material:"<<i<<std::endl;
                return i;
            }
        }
        return -1;
    }
private:
    bool has_texture = false;
};
