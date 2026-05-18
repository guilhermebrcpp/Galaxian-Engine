
#include "objParser.h"
#include "mesh.h"
#include "texture.h"

bool starts_with(std::string starter, std::string str){
    for(int i = 0; i < starter.length(); i++){
        if(starter[i] != str[i])
            return false;
    }
    return true;
}

//load obj file at given path
void load_model(std::string file_path, mesh *m){

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
                            m->vertices_x.push_back(std::stof(number));
                            break;
                        case 1:
                            m->vertices_y.push_back(std::stof(number));
                            break;
                        case 2:
                            m->vertices_z.push_back(std::stof(number));
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
                        if(number.length() != 0)
                            sub_texture_mesh.push_back(std::stoi(number));
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
            m->vertex_texture.push_back(new_vec2);
        }
        else if(starts_with("usemtl ", line)){
            if(sub_mesh.size() != 0){
                m->triangles.push_back(sub_mesh);
                sub_mesh.clear();
            }
            if(sub_texture_mesh.size() != 0)
                m->vertex_texture_indices.push_back(sub_texture_mesh);

            sub_texture_mesh.clear();

            std::string name;
            for(int i = 7; i < line.length(); i++){
                name += line[i];
            }
            m->materials_used_in_order.push_back(name);
        }
    }
    m->vertex_texture_indices.push_back(sub_texture_mesh);
    m->triangles.push_back(sub_mesh);
    inputFile.close();

    int triangle_quantity = 0;
    for(int i = 0; i < m->triangles.size(); i++){
        triangle_quantity += m->triangles[i].size()/3;
    }
    std::cout<<"texture vertex quantity:"<<m->vertex_texture.size()<<std::endl;
    std::cout<<"vertex quantity:"<<m->vertices_x.size() + m->vertices_y.size() + m->vertices_z.size()<<std::endl;
    std::cout<<"triangle submesh quantity:"<<m->triangles.size()<<std::endl;
    std::cout<<"triangle quantity:"<<triangle_quantity<<std::endl;
}

void load_texture(std::string file_path, texture *tex, mesh *m){

    std::ifstream inputFile(file_path);

    //check if the file was oppened succefully
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file at: \""<<file_path<<"\"!."<< std::endl;
        system("pause");
        return;
    }

    std::string line;

    while (std::getline(inputFile, line)) {
        tex->data += line;
        tex->set_width(line.length());
    }

    inputFile.close();

    tex->set_height(tex->data.length()/tex->get_width());

    m->has_texture = true;
}

void load_material(std::string file_path, mesh *m){
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
                m->materials.push_back(new_material);
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
    m->materials.push_back(new_material);
    inputFile.close();

    std::cout<<"materials quantity:"<<m->materials.size()<<std::endl;
    for(int i = 0; i < m->materials.size(); i++){
        //std::cout<<"nome do material:"<<materials[i].name<<std::endl;
        //std::cout<<"nome da textura:"<<materials[i].texture_file_name<<std::endl;
    }

    //load all textures for the submaterials
    for(int i = 0; i < m->materials.size(); i++){
        if(m->materials[i].texture_file_name.length() == 0){
            m->materials[i].albedo_texture.data += "....";
            m->materials[i].albedo_texture.data += "....";
            m->materials[i].albedo_texture.data += "....";
            m->materials[i].albedo_texture.data += "....";
            continue;
        }

        m->materials[i].texture_file_name.replace(m->materials[i].texture_file_name.length()-3, 3, "txt");
        //std::cout<<"nome blabla:"<<materials[i].texture_file_name<<std::endl;
        load_texture("textures\\" + m->materials[i].texture_file_name, &m->materials[i].albedo_texture, m);
    }
}
